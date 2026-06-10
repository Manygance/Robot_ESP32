#include <Arduino.h>
#include <Wire.h>
#include "MotorControl.h"
#include "SparkFun_Qwiic_OTOS_Arduino_Library.h"

// Déclaration de votre fonction d'interface Web
extern void initWebInterface();

// --- BROCHES I2C ---
#define I2C_SDA_PIN 8
#define I2C_SCL_PIN 9

// Création de l'objet Capteur
QwiicOTOS myOtos;

// Fonction globale pour remettre le capteur à zéro
void resetOdometry() {
  myOtos.calibrateImu();
  myOtos.resetTracking();
  Serial.println("\n[!] Odométrie réinitialisée à 0 (X=0, Y=0, Angle=0)\n");
}

void setup() {
  Serial.begin(115200);
  delay(1000); // Laisse le temps au terminal de s'ouvrir

  // 1. Démarrage du bus I2C sur les broches 8 et 9
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);

  // 2. Initialisation matérielle du robot
  initMotors();
  initWebInterface(); 

  // 3. Initialisation du capteur OTOS
  Serial.println("\n--- RECHERCHE DU CAPTEUR OTOS ---");
  
  if (myOtos.begin() == false) {
    Serial.println("ERREUR : Capteur introuvable !");
    Serial.println("Vérifiez que SDA est sur la broche 8 et SCL sur la broche 9.");
  } else {
    Serial.println("Capteur OTOS connecté avec succès.");
    
    // On configure en Mètres (la librairie ne gère pas les mm directement)
    myOtos.setLinearUnit(kSfeOtosLinearUnitMeters);
    myOtos.setAngularUnit(kSfeOtosAngularUnitDegrees);

    myOtos.setLinearScalar(0.9801);

    Serial.println("Calibration de l'IMU en cours... Ne touchez pas au robot !");
    myOtos.calibrateImu(); 
    
    resetOdometry();
    Serial.println("Calibration terminée. Lecture en cours...\n");
  }
}

void loop() {
  // Gestion continue des moteurs
  updateMotors(); 

  // --- ÉCOUTE DU CLAVIER (VSCODE SERIAL MONITOR) ---
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n'); // Lit jusqu'à la touche Entrée
    input.trim(); // Enlève les espaces invisibles
    if (input.length() > 0) {
      traiterCommande(input); // Envoie la commande tapée au cerveau du robot
    }
  }

  // --- LECTURE DU CAPTEUR OPTIQUE ---
  static unsigned long lastOtosPrint = 0;
  if (millis() - lastOtosPrint >= 100) {
    lastOtosPrint = millis();

    sfe_otos_pose2d_t myPosition;
    myOtos.getPosition(myPosition);

    // On multiplie par 1000.0 pour convertir les Mètres en Millimètres
    Serial.printf("POS -> X: %7.1f mm  |  Y: %7.1f mm  |  Angle: %6.1f deg\n", 
                  myPosition.x * 1000.0, 
                  myPosition.y * 1000.0, 
                  myPosition.h);
  }
}