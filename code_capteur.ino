// === Déclaration des broches ===
const int IN1 = 4;  // Moteur droit - Direction
const int IN2 = 5;  // Moteur droit - Direction
const int IN3 = 6;  // Moteur gauche - Direction
const int IN4 = 7;  // Moteur gauche - Direction
const int ENA = 9;  // Moteur droit - Vitesse (PWM)
const int ENB = 10; // Moteur gauche - Vitesse (PWM)
const int TRIG = 13; // Capteur à ultrasons - Trigger
const int ECHO = 12; // Capteur à ultrasons - Echo

// === Configuration pour inverser les moteurs si nécessaire ===
const bool INVERT_MOTORS = false; // Changez à true pour inverser le sens des moteurs

// === Paramètres ===
const int DISTANCE_MIN = 20; // Distance minimale pour détecter un obstacle (cm)
const int VITESSE = 100;     // Vitesse par défaut pour les moteurs

// === Fonction pour mesurer la distance avec le capteur à ultrasons ===
long getDistance() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long duration = pulseIn(ECHO, HIGH);
  long distance = duration * 0.034 / 2; // Conversion en cm
  return distance;
}

// === Fonctions de contrôle moteur avec vitesse individuelle ===
void avancer(int vitesseDroit, int vitesseGauche, int duree) {
  Serial.print("Avancer - Droit:");
  Serial.print(vitesseDroit);
  Serial.print(" / Gauche:");
  Serial.println(vitesseGauche);

  analogWrite(ENA, vitesseDroit);
  analogWrite(ENB, vitesseGauche);

  if (INVERT_MOTORS) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  } else {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  }

  delay(duree);
}

void reculer(int vitesseDroit, int vitesseGauche, int duree) {
  Serial.print("Reculer - Droit:");
  Serial.print(vitesseDroit);
  Serial.print(" / Gauche:");
  Serial.println(vitesseGauche);

  analogWrite(ENA, vitesseDroit);
  analogWrite(ENB, vitesseGauche);

  if (INVERT_MOTORS) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  }

  delay(duree);
}

void tournerGauche(int vitesse, int duree) {
  Serial.print("Tourner à gauche - Vitesse:");
  Serial.println(vitesse);

  analogWrite(ENA, vitesse); // Moteur droit avance
  analogWrite(ENB, 0);       // Moteur gauche arrêté

  if (INVERT_MOTORS) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
  } else {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
  }

  delay(duree);
}

void tournerDroite(int vitesse, int duree) {
  Serial.print("Tourner à droite - Vitesse:");
  Serial.println(vitesse);

  analogWrite(ENA, 0);       // Moteur droit arrêté
  analogWrite(ENB, vitesse); // Moteur gauche avance

  if (INVERT_MOTORS) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  }

  delay(duree);
}

void arreter(int duree) {
  Serial.println("Arrêt");

  analogWrite(ENA, 0);
  analogWrite(ENB, 0);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  delay(duree);
}

// === Initialisation ===
void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  Serial.begin(9600);
  Serial.println("=== Contrôle de la voiture robotisée avec évitement d'obstacles ===");

  arreter(1000); // Arrêt initial
  randomSeed(analogRead(0)); // Initialiser le générateur aléatoire
}

// === Boucle principale ===
void loop() {
  // Mesurer la distance
  long distance = getDistance();
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Vérifier s'il y a un obstacle
  if (distance < DISTANCE_MIN && distance > 0) {
    // Obstacle détecté
    reculer(VITESSE, VITESSE, 1000); // Reculer pendant 1 seconde
    arreter(500);                    // Arrêter brièvement

    // Choisir aléatoirement de tourner à gauche ou à droite
    if (random(2) == 0) {
      tournerGauche(VITESSE, 1000); // Tourner à gauche 1 seconde
    } else {
      tournerDroite(VITESSE, 1000); // Tourner à droite 1 seconde
    }
    arreter(500); // Arrêter brièvement
  } else {
    // Pas d'obstacle, avancer
    avancer(VITESSE, VITESSE, 1000); // Avancer pendant 1 seconde
  }
}
