Parfait ! 🔥

Je vais te préparer **un serveur complet en Python avec Flask** qui fait tout ce que tu demandes :
- 🔹 Il **reçoit une commande** (par requête HTTP POST depuis ton application web)
- 🔹 Il **génère dynamiquement** un fichier `.ino`
- 🔹 Il **compile** le code pour l'Arduino Mega
- 🔹 Il **téléverse** le code vers l'Arduino connecté à la Raspberry Pi

---

# 🚀 Voici le projet complet (Python/Flask) :

---

## 📁 Arborescence du projet

```
/car-controller/
├── server.py
├── sketches/
│    └── sketch.ino
├── requirements.txt
└── README.md
```

---

## 🛠️ 1. `requirements.txt`

```text
flask
```

---

## 🛠️ 2. `server.py`

```python
from flask import Flask, request, jsonify
import subprocess
import os

app = Flask(__name__)

# Dossier pour stocker le fichier .ino
SKETCH_DIR = "/home/pi/car-controller/sketches"
SKETCH_FILE = os.path.join(SKETCH_DIR, "sketch.ino")

# Configuration Arduino
BOARD_FQBN = "arduino:avr:mega"
PORT = "/dev/ttyACM0"  # À ajuster selon ton branchement USB

# Crée le dossier au cas où il n'existe pas
os.makedirs(SKETCH_DIR, exist_ok=True)

def generate_arduino_code(command):
    code = """
void setup() {
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
}

void loop() {
"""
    if command == "forward":
        code += "  digitalWrite(9, HIGH);\n  digitalWrite(10, LOW);\n"
    elif command == "backward":
        code += "  digitalWrite(9, LOW);\n  digitalWrite(10, HIGH);\n"
    elif command == "stop":
        code += "  digitalWrite(9, LOW);\n  digitalWrite(10, LOW);\n"
    else:
        code += "  // Commande inconnue\n"

    code += """
  delay(100);
}
"""
    with open(SKETCH_FILE, "w") as f:
        f.write(code)

def compile_sketch():
    result = subprocess.run(
        ["arduino-cli", "compile", "--fqbn", BOARD_FQBN, SKETCH_DIR],
        capture_output=True,
        text=True
    )
    return result

def upload_sketch():
    result = subprocess.run(
        ["arduino-cli", "upload", "-p", PORT, "--fqbn", BOARD_FQBN, SKETCH_DIR],
        capture_output=True,
        text=True
    )
    return result

@app.route('/command', methods=['POST'])
def receive_command():
    data = request.json
    command = data.get('command')

    if not command:
        return jsonify({"error": "No command received"}), 400

    # Générer le sketch
    generate_arduino_code(command)

    # Compiler
    compile_result = compile_sketch()
    if compile_result.returncode != 0:
        return jsonify({"error": "Compilation failed", "details": compile_result.stderr}), 500

    # Téléverser
    upload_result = upload_sketch()
    if upload_result.returncode != 0:
        return jsonify({"error": "Upload failed", "details": upload_result.stderr}), 500

    return jsonify({"success": True, "message": f"Command '{command}' sent to Arduino"}), 200

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
```

---

## 📜 Explication rapide

- Quand ton **application web** fait un `POST` à `http://raspberry-pi-ip:5000/command` avec `{ "command": "forward" }` :
  - le serveur :
    1. génère un fichier `.ino`
    2. compile le code avec `arduino-cli`
    3. téléverse directement dans l’Arduino Mega.
- S'il y a une erreur (compilation ou téléversement), il renvoie une réponse JSON détaillée.

---

## 📡 Exemple de requête HTTP côté Application Web (Javascript)

```javascript
fetch('http://raspberry-pi-ip:5000/command', {
  method: 'POST',
  headers: {
    'Content-Type': 'application/json'
  },
  body: JSON.stringify({ command: "forward" })
})
.then(response => response.json())
.then(data => console.log(data))
.catch(error => console.error('Erreur:', error));
```

---

# 🧩 À préparer avant de tester
1. **Installer Flask** sur la Raspberry :

```bash
pip install flask
```

2. **Installer `arduino-cli`** et configurer :

```bash
curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh
arduino-cli config init
arduino-cli core update-index
arduino-cli core install arduino:avr
```

3. **Vérifier le port de ton Arduino** avec :

```bash
ls /dev/tty*
```
Souvent `/dev/ttyACM0` pour Arduino Mega.

---

# ⚡ Bonus si besoin
- Possibilité de **faire des commandes plus complexes** (ex: vitesse moteur variable, rotation de roue, marche arrière avec vitesse).
- Possibilité de **générer plusieurs fichiers** (.h, .cpp) si tu veux aller encore plus loin.
- Ajout d'une **sécurité** pour éviter de téléverser n'importe quoi.

---

# 🚀 **Veux-tu que je te prépare aussi :**
- ✅ Un exemple avec plusieurs **vitesse de moteur** (PWM, analogWrite) ?
- ✅ Une version **multi-commandes** (avancer 2s, tourner à droite, reculer) ?
- ✅ Un **fichier `.service`** pour que ton serveur Flask démarre automatiquement au démarrage de la Raspberry Pi ?  

👉 Dis-moi ce que tu veux pour aller encore plus loin avec ton projet ! 🚗🔥  
On peut faire ta voiture **autonome** ou **semi-autonome** si tu veux après.
