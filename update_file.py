from flask import Flask, request, jsonify
import subprocess
import os
import time
import logging

# Configuration des logs
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    handlers=[
        logging.FileHandler("/home/pi/car-controller/server.log"),
        logging.StreamHandler()
    ]
)

logger = logging.getLogger("car-controller")

app = Flask(__name__)

# Dossier pour stocker le fichier .ino
SKETCH_DIR = "/home/pi/car-controller/sketches"
SKETCH_FILE = os.path.join(SKETCH_DIR, "sketch.ino")

# Configuration Arduino
BOARD_FQBN = "arduino:avr:mega"
PORT = "/dev/ttyACM0"  # À ajuster selon ton branchement USB

# Crée le dossier au cas où il n'existe pas
os.makedirs(SKETCH_DIR, exist_ok=True)

def generate_arduino_code(command, speed=200):
    """
    Génère le code Arduino complet avec les bibliothèques et fonctions nécessaires
    """
    # En-tête avec importations et définitions des broches
    code = """// Code généré automatiquement pour contrôle de voiture
#include <Arduino.h>

// Définition des broches
#define PIN_MOTEUR_DROITE_AVANT 9
#define PIN_MOTEUR_DROITE_ARRIERE 10
#define PIN_MOTEUR_GAUCHE_AVANT 6
#define PIN_MOTEUR_GAUCHE_ARRIERE 7

// Variables globales
int vitesse = {speed}; // Valeur PWM entre 0-255

void setup() {
  // Initialisation de la communication série
  Serial.begin(9600);
  Serial.println("Initialisation du contrôleur de voiture");
  
  // Configuration des broches en mode sortie
  pinMode(PIN_MOTEUR_DROITE_AVANT, OUTPUT);
  pinMode(PIN_MOTEUR_DROITE_ARRIERE, OUTPUT);
  pinMode(PIN_MOTEUR_GAUCHE_AVANT, OUTPUT);
  pinMode(PIN_MOTEUR_GAUCHE_ARRIERE, OUTPUT);
  
  // Initialisation - tous les moteurs à l'arrêt
  analogWrite(PIN_MOTEUR_DROITE_AVANT, 0);
  analogWrite(PIN_MOTEUR_DROITE_ARRIERE, 0);
  analogWrite(PIN_MOTEUR_GAUCHE_AVANT, 0);
  analogWrite(PIN_MOTEUR_GAUCHE_ARRIERE, 0);
  
  Serial.println("Prêt à recevoir des commandes");
}

""".format(speed=speed)

    # Ajouter des fonctions utilitaires pour chaque mouvement
    code += """
// Fonctions de mouvement
void avancer(int vitesse_pwm) {
  analogWrite(PIN_MOTEUR_DROITE_AVANT, vitesse_pwm);
  analogWrite(PIN_MOTEUR_DROITE_ARRIERE, 0);
  analogWrite(PIN_MOTEUR_GAUCHE_AVANT, vitesse_pwm);
  analogWrite(PIN_MOTEUR_GAUCHE_ARRIERE, 0);
  Serial.println("Avance");
}

void reculer(int vitesse_pwm) {
  analogWrite(PIN_MOTEUR_DROITE_AVANT, 0);
  analogWrite(PIN_MOTEUR_DROITE_ARRIERE, vitesse_pwm);
  analogWrite(PIN_MOTEUR_GAUCHE_AVANT, 0);
  analogWrite(PIN_MOTEUR_GAUCHE_ARRIERE, vitesse_pwm);
  Serial.println("Recule");
}

void arreter() {
  analogWrite(PIN_MOTEUR_DROITE_AVANT, 0);
  analogWrite(PIN_MOTEUR_DROITE_ARRIERE, 0);
  analogWrite(PIN_MOTEUR_GAUCHE_AVANT, 0);
  analogWrite(PIN_MOTEUR_GAUCHE_ARRIERE, 0);
  Serial.println("Arrêt");
}

void tournerDroite(int vitesse_pwm) {
  analogWrite(PIN_MOTEUR_DROITE_AVANT, 0);
  analogWrite(PIN_MOTEUR_DROITE_ARRIERE, vitesse_pwm);
  analogWrite(PIN_MOTEUR_GAUCHE_AVANT, vitesse_pwm);
  analogWrite(PIN_MOTEUR_GAUCHE_ARRIERE, 0);
  Serial.println("Tourne à droite");
}

void tournerGauche(int vitesse_pwm) {
  analogWrite(PIN_MOTEUR_DROITE_AVANT, vitesse_pwm);
  analogWrite(PIN_MOTEUR_DROITE_ARRIERE, 0);
  analogWrite(PIN_MOTEUR_GAUCHE_AVANT, 0);
  analogWrite(PIN_MOTEUR_GAUCHE_ARRIERE, vitesse_pwm);
  Serial.println("Tourne à gauche");
}
"""

    # Fonction loop principale
    code += """
void loop() {
"""
    
    # Ajouter le code spécifique à la commande
    if command == "forward":
        code += "  avancer(vitesse);\n"
    elif command == "backward":
        code += "  reculer(vitesse);\n"
    elif command == "stop":
        code += "  arreter();\n"
    elif command == "right":
        code += "  tournerDroite(vitesse);\n"
    elif command == "left":
        code += "  tournerGauche(vitesse);\n"
    else:
        code += "  arreter(); // Commande inconnue\n"

    code += """
  delay(100); // Petit délai pour stabilité
}
"""
    
    with open(SKETCH_FILE, "w") as f:
        f.write(code)
    
    logger.info(f"Code Arduino généré pour la commande: {command}")
    return True

def compile_sketch():
    """
    Compile le sketch Arduino
    """
    logger.info("Compilation du sketch Arduino...")
    result = subprocess.run(
        ["arduino-cli", "compile", "--fqbn", BOARD_FQBN, SKETCH_DIR],
        capture_output=True,
        text=True
    )
    
    if result.returncode != 0:
        logger.error(f"Erreur de compilation: {result.stderr}")
    else:
        logger.info("Compilation réussie!")
        
    return result

def upload_sketch():
    """
    Téléverse le sketch compilé vers l'Arduino
    """
    logger.info("Téléversement du sketch vers l'Arduino...")
    result = subprocess.run(
        ["arduino-cli", "upload", "-p", PORT, "--fqbn", BOARD_FQBN, SKETCH_DIR],
        capture_output=True,
        text=True
    )
    
    if result.returncode != 0:
        logger.error(f"Erreur de téléversement: {result.stderr}")
    else:
        logger.info("Téléversement réussi!")
        
    return result

@app.route('/command', methods=['POST'])
def receive_command():
    """
    Endpoint pour recevoir les commandes de l'application web
    """
    data = request.json
    if not data:
        return jsonify({"error": "No JSON data received"}), 400
        
    command = data.get('command')
    speed = data.get('speed', 200)  # Vitesse par défaut si non spécifiée

    if not command:
        return jsonify({"error": "No command received"}), 400

    logger.info(f"Commande reçue: {command}, vitesse: {speed}")

    # Validation des commandes
    valid_commands = ["forward", "backward", "stop", "left", "right"]
    if command not in valid_commands:
        return jsonify({"error": "Invalid command", "valid_commands": valid_commands}), 400

    # Validation de la vitesse
    if not isinstance(speed, int) or speed < 0 or speed > 255:
        return jsonify({"error": "Speed must be an integer between 0 and 255"}), 400

    # Générer le sketch
    generate_arduino_code(command, speed)

    # Compiler
    compile_result = compile_sketch()
    if compile_result.returncode != 0:
        return jsonify({
            "error": "Compilation failed", 
            "details": compile_result.stderr
        }), 500

    # Téléverser
    upload_result = upload_sketch()
    if upload_result.returncode != 0:
        return jsonify({
            "error": "Upload failed", 
            "details": upload_result.stderr
        }), 500

    return jsonify({
        "success": True, 
        "message": f"Command '{command}' with speed {speed} sent to Arduino"
    }), 200

@app.route('/status', methods=['GET'])
def status():
    """
    Endpoint pour vérifier l'état du serveur
    """
    # Vérifier si l'Arduino est connecté
    result = subprocess.run(
        ["arduino-cli", "board", "list"],
        capture_output=True,
        text=True
    )
    
    arduino_connected = PORT in result.stdout
    
    return jsonify({
        "status": "running",
        "arduino_connected": arduino_connected,
        "port": PORT,
        "board": BOARD_FQBN
    })

if __name__ == '__main__':
    logger.info("Démarrage du serveur Flask pour le contrôle de voiture...")
    app.run(host='0.0.0.0', port=5000, debug=False)
