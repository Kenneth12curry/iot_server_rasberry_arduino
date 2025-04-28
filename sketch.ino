// Code corrigé pour contrôle de voiture
#include <Arduino.h>

// Définition des broches pour le pilote de moteur (utiliser des broches PWM)
#define PIN_MOTEUR_DROITE_AVANT 4   // IN1 pour le moteur droit (OUT1)
#define PIN_MOTEUR_DROITE_ARRIERE 5 // IN2 pour le moteur droit (OUT2)
#define PIN_MOTEUR_GAUCHE_AVANT 6   // IN3 pour le moteur gauche (OUT3)
#define PIN_MOTEUR_GAUCHE_ARRIERE 7 // IN4 pour le moteur gauche (OUT4)

// Variables globales
int vitesse = 200; // Valeur PWM entre 0-255

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

// Fonctions de mouvement pour les moteurs DC
void avancer(int vitesse_pwm) {
  // Moteur droit - avant
  analogWrite(PIN_MOTEUR_DROITE_AVANT, vitesse_pwm);
  analogWrite(PIN_MOTEUR_DROITE_ARRIERE, 0);
 
  // Moteur gauche - avant
  analogWrite(PIN_MOTEUR_GAUCHE_AVANT, vitesse_pwm);
  analogWrite(PIN_MOTEUR_GAUCHE_ARRIERE, 0);
 
  Serial.print("Avance à la vitesse: ");
  Serial.println(vitesse_pwm);
}

void reculer(int vitesse_pwm) {
  // Moteur droit - arrière
  analogWrite(PIN_MOTEUR_DROITE_AVANT, 0);
  analogWrite(PIN_MOTEUR_DROITE_ARRIERE, vitesse_pwm);
 
  // Moteur gauche - arrière
  analogWrite(PIN_MOTEUR_GAUCHE_AVANT, 0);
  analogWrite(PIN_MOTEUR_GAUCHE_ARRIERE, vitesse_pwm);
 
  Serial.print("Recule à la vitesse: ");
  Serial.println(vitesse_pwm);
}

void arreter() {
  // Arrêt complet des deux moteurs
  analogWrite(PIN_MOTEUR_DROITE_AVANT, 0);
  analogWrite(PIN_MOTEUR_DROITE_ARRIERE, 0);
  analogWrite(PIN_MOTEUR_GAUCHE_AVANT, 0);
  analogWrite(PIN_MOTEUR_GAUCHE_ARRIERE, 0);
 
  Serial.println("Arrêt des moteurs");
}

void tournerDroite(int vitesse_pwm) {
  // Pour tourner à droite:
  // - Le moteur gauche avance
  // - Le moteur droit recule ou est arrêté
  analogWrite(PIN_MOTEUR_DROITE_AVANT, 0);
  analogWrite(PIN_MOTEUR_DROITE_ARRIERE, vitesse_pwm);
  analogWrite(PIN_MOTEUR_GAUCHE_AVANT, vitesse_pwm);
  analogWrite(PIN_MOTEUR_GAUCHE_ARRIERE, 0);
 
  Serial.print("Tourne à droite à la vitesse: ");
  Serial.println(vitesse_pwm);
}

void tournerGauche(int vitesse_pwm) {
  // Pour tourner à gauche:
  // - Le moteur droit avance
  // - Le moteur gauche recule ou est arrêté
  analogWrite(PIN_MOTEUR_DROITE_AVANT, vitesse_pwm);
  analogWrite(PIN_MOTEUR_DROITE_ARRIERE, 0);
  analogWrite(PIN_MOTEUR_GAUCHE_AVANT, 0);
  analogWrite(PIN_MOTEUR_GAUCHE_ARRIERE, vitesse_pwm);
 
  Serial.print("Tourne à gauche à la vitesse: ");
  Serial.println(vitesse_pwm);
}

void loop() {
  avancer(vitesse);
  delay(2000); // Avance pendant 2 secondes
  arreter();
  delay(1000); // Pause de 1 seconde
  tournerDroite(vitesse);
  delay(1000); // Tourne pendant 1 seconde
  arreter();
  delay(1000); // Pause de 1 seconde
}
