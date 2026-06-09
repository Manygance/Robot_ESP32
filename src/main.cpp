#include <Arduino.h>
#include "Config.h"
#include "MotorControl.h"
#include "WebInterface.h"

void setup() {
  Serial.begin(115200);
  delay(2000);

  // 1. Matériel sur le Cœur 1
  initMotors();

  // 2. Wi-Fi et OTA sur le Cœur 0
  initWebInterface();
}

void loop() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    traiterCommande(input);
  }
  // Actualisation de la boucle physique
  updateMotors();
}