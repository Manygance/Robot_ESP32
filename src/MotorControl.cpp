#include "MotorControl.h"
#include "Config.h"
#include <ESP32Encoder.h>

volatile float ticksPerRevolution = 340.0; 

// --- VARIABLES PID ---
float Kp = 0.5;  
float Ki = 0.0;  
float Kd = 0.0;  

const float ACCEL_MMS2 = 1500.0; 

bool isDistanceModeM1 = false;
bool isDistanceModeM2 = false;

float targetPosM1_mm = 0.0, targetPosM2_mm = 0.0;
float currentSpeedM1_mms = 0.0, currentSpeedM2_mms = 0.0;
float finalTargetPosM1_mm = 0.0, finalTargetPosM2_mm = 0.0;
float reqSpeedM1_mms = 0.0, reqSpeedM2_mms = 0.0;
float maxSpeedDistM1 = 0.0, maxSpeedDistM2 = 0.0;

float integralM1 = 0, integralM2 = 0;
float lastErrorM1 = 0, lastErrorM2 = 0;
unsigned long lastPidTime = 0;

volatile float globalSpeedM1 = 0.0, globalSpeedM2 = 0.0; 
int currentPwmM1 = 0, currentPwmM2 = 0; // Variables ajoutées pour l'affichage du PWM

unsigned long lastSpeedTime = 0;
const unsigned long SPEED_INTERVAL = 50; 
long lastCountM1 = 0, lastCountM2 = 0;

ESP32Encoder encoderM1;
ESP32Encoder encoderM2;

// --- GETTERS ---
float getSpeedM1() { return globalSpeedM1; }
float getSpeedM2() { return globalSpeedM2; }
int getPwmM1() { return currentPwmM1; } 
int getPwmM2() { return currentPwmM2; }
float getTicksPerRevolution() { return ticksPerRevolution; }
void setTicksPerRevolution(float val) { ticksPerRevolution = val; }

void updateMotorHardware(int pwmValue, int dirPin, int pwmPin, bool forwardState) {
  bool dirState = (pwmValue >= 0) ? forwardState : !forwardState; 
  digitalWrite(dirPin, dirState);
  ledcWrite(pwmPin, abs(pwmValue)); 
}

void initMotors() {
  pinMode(M1_DIR_PIN, OUTPUT); pinMode(M2_DIR_PIN, OUTPUT);
  ledcAttach(M1_PWM_PIN, 5000, 8); ledcAttach(M2_PWM_PIN, 5000, 8);
  
  ESP32Encoder::useInternalWeakPullResistors = puType::up;
  encoderM1.attachFullQuad(M1_ENCA_PIN, M1_ENCB_PIN); 
  encoderM2.attachFullQuad(M2_ENCA_PIN, M2_ENCB_PIN);
  encoderM1.clearCount(); encoderM2.clearCount();
}

void setVelocity(int motorId, float speed_mms) {
  if (motorId == 1 || motorId == 0) { isDistanceModeM1 = false; reqSpeedM1_mms = speed_mms; }
  if (motorId == 2 || motorId == 0) { isDistanceModeM2 = false; reqSpeedM2_mms = speed_mms; }
}

void moveDistance(int motorId, float distance_mm, float speed_mms) {
  if (motorId == 1 || motorId == 0) {
    isDistanceModeM1 = true;
    finalTargetPosM1_mm = targetPosM1_mm + distance_mm;
    maxSpeedDistM1 = abs(speed_mms);
  }
  if (motorId == 2 || motorId == 0) {
    isDistanceModeM2 = true;
    finalTargetPosM2_mm = targetPosM2_mm + distance_mm;
    maxSpeedDistM2 = abs(speed_mms);
  }
}

void traiterCommande(String input) {
  input.trim();
  if (input.length() > 0) {
    int motorId; char dirChar; int speedVal; char keyword[10] = ""; float value = 0;
    if (sscanf(input.c_str(), "%d %c %d %9s %f", &motorId, &dirChar, &speedVal, keyword, &value) >= 3) {
      speedVal = constrain(speedVal, 0, (int)MAX_SPEED_MMS);
      int currentDir = (dirChar == 'A' || dirChar == 'a') ? 1 : -1;
      
      if (speedVal == 0) { setVelocity(motorId, 0.0); return; }

      String kw = String(keyword); kw.toLowerCase();
      if (kw == "tour" || kw == "tours") { moveDistance(motorId, value, speedVal); } 
      else { setVelocity(motorId, speedVal * currentDir); }
    }
  }
}

void updateProfile(bool isDistanceMode, float &targetPos, float finalTargetPos, float &currentSpeed, float reqSpeed, float maxSpeedDist, float dt) {
  if (isDistanceMode) {
    float distanceToGo = finalTargetPos - targetPos;
    float dir = (distanceToGo > 0) ? 1.0 : -1.0;

    if (abs(distanceToGo) < 1.0) { 
      currentSpeed = 0.0; targetPos = finalTargetPos;
    } else {
      float stopDist = (currentSpeed * currentSpeed) / (2.0 * ACCEL_MMS2);
      if (abs(distanceToGo) <= stopDist) { currentSpeed -= dir * ACCEL_MMS2 * dt; } 
      else { currentSpeed += dir * ACCEL_MMS2 * dt; if (abs(currentSpeed) > maxSpeedDist) currentSpeed = dir * maxSpeedDist; }
    }
  } else {
    if (currentSpeed < reqSpeed) { currentSpeed += ACCEL_MMS2 * dt; if (currentSpeed > reqSpeed) currentSpeed = reqSpeed; } 
    else if (currentSpeed > reqSpeed) { currentSpeed -= ACCEL_MMS2 * dt; if (currentSpeed < reqSpeed) currentSpeed = reqSpeed; }
  }
  targetPos += currentSpeed * dt;
}

void computePID() {
  unsigned long now = millis();
  float dt = (now - lastPidTime) / 1000.0; 
  if (dt < 0.01) return; 
  lastPidTime = now;

  updateProfile(isDistanceModeM1, targetPosM1_mm, finalTargetPosM1_mm, currentSpeedM1_mms, reqSpeedM1_mms, maxSpeedDistM1, dt);
  updateProfile(isDistanceModeM2, targetPosM2_mm, finalTargetPosM2_mm, currentSpeedM2_mms, reqSpeedM2_mms, maxSpeedDistM2, dt);

  float currentPosM1_mm = (encoderM1.getCount() * M1_ENCODER_DIR) / TICKS_PER_MM;
  float currentPosM2_mm = (encoderM2.getCount() * M2_ENCODER_DIR) / TICKS_PER_MM;

  float errorM1 = targetPosM1_mm - currentPosM1_mm;
  float errorM2 = targetPosM2_mm - currentPosM2_mm;

  integralM1 += errorM1 * dt;
  float derivativeM1 = (errorM1 - lastErrorM1) / dt;
  float outputM1 = (Kp * errorM1) + (Ki * integralM1) + (Kd * derivativeM1);
  lastErrorM1 = errorM1;

  integralM2 += errorM2 * dt;
  float derivativeM2 = (errorM2 - lastErrorM2) / dt;
  float outputM2 = (Kp * errorM2) + (Ki * integralM2) + (Kd * derivativeM2);
  lastErrorM2 = errorM2;

  // On sauvegarde le PWM réel avant de l'envoyer
  currentPwmM1 = constrain((int)outputM1, -255, 255);
  currentPwmM2 = constrain((int)outputM2, -255, 255);

  updateMotorHardware(currentPwmM1, M1_DIR_PIN, M1_PWM_PIN, M1_FORWARD_STATE);
  updateMotorHardware(currentPwmM2, M2_DIR_PIN, M2_PWM_PIN, M2_FORWARD_STATE);
}

void updateMotors() {
  computePID(); 

  unsigned long currentMillis = millis();
  if (currentMillis - lastSpeedTime >= SPEED_INTERVAL) {
    lastSpeedTime = currentMillis;
    long ticksM1 = encoderM1.getCount() * M1_ENCODER_DIR; 
    long ticksM2 = encoderM2.getCount() * M2_ENCODER_DIR;
    long rawSpeedM1 = ticksM1 - lastCountM1; 
    long rawSpeedM2 = ticksM2 - lastCountM2;
    lastCountM1 = ticksM1; lastCountM2 = ticksM2;

    globalSpeedM1 = (rawSpeedM1 * (1000.0 / SPEED_INTERVAL)) / TICKS_PER_MM;
    globalSpeedM2 = (rawSpeedM2 * (1000.0 / SPEED_INTERVAL)) / TICKS_PER_MM;

    // Nouvel affichage épuré
    static unsigned long lastSerialPrint = 0;
    if (currentMillis - lastSerialPrint >= 200) {
      lastSerialPrint = currentMillis;
      Serial.printf("PWM M1: %4d | Vit M1: %4.0f mm/s || PWM M2: %4d | Vit M2: %4.0f mm/s\n", 
                    currentPwmM1, globalSpeedM1, currentPwmM2, globalSpeedM2);
    }
  }
}