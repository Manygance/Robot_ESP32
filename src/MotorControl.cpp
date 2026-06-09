#include "MotorControl.h"
#include "Config.h"
#include <ESP32Encoder.h>

volatile float ticksPerRevolution = 340.0; 

volatile int currentSpeedM1 = 0, targetSpeedM1 = 0, requestedSpeedM1 = 0;
volatile int currentSpeedM2 = 0, targetSpeedM2 = 0, requestedSpeedM2 = 0;
unsigned long lastRampTime = 0;

volatile bool isPositionModeM1 = false, isPositionModeM2 = false;
volatile long startTicksM1 = 0, profileDtotalM1 = 0, profileDaccelM1 = 0, profileDdecelM1 = 0;
volatile long startTicksM2 = 0, profileDtotalM2 = 0, profileDaccelM2 = 0, profileDdecelM2 = 0;
volatile int profileVmaxM1 = 0, profilePhaseM1 = 1;
volatile int profileVmaxM2 = 0, profilePhaseM2 = 1;

volatile bool isTimeModeM1 = false, isTimeModeM2 = false;
volatile unsigned long timeStartM1 = 0, timeStartM2 = 0;
volatile long timeDurationM1 = 0, timeTdecelM1 = 0;
volatile long timeDurationM2 = 0, timeTdecelM2 = 0;
volatile int timeVmaxM1 = 0, timeVmaxM2 = 0;

volatile float globalRpmM1 = 0.0, globalRpmM2 = 0.0;

ESP32Encoder encoderM1;
ESP32Encoder encoderM2;
unsigned long lastSpeedTime = 0;
const unsigned long SPEED_INTERVAL = 50; 
long lastCountM1 = 0, lastCountM2 = 0;

float getRpmM1() { return globalRpmM1; }
float getRpmM2() { return globalRpmM2; }
float getTicksPerRevolution() { return ticksPerRevolution; }
void setTicksPerRevolution(float val) { ticksPerRevolution = val; }

void updateMotorHardware(int currentSpeed, int dirPin, int pwmPin, bool forwardState) {
  int pwmValue = map(abs(currentSpeed), 0, 100, 0, 255);
  bool dirState = (currentSpeed >= 0) ? forwardState : !forwardState; 
  digitalWrite(dirPin, dirState);
  ledcWrite(pwmPin, pwmValue); 
}

void initMotors() {
  pinMode(M1_DIR_PIN, OUTPUT); pinMode(M2_DIR_PIN, OUTPUT);
  ledcAttach(M1_PWM_PIN, 5000, 8); ledcAttach(M2_PWM_PIN, 5000, 8);
  ESP32Encoder::useInternalWeakPullResistors = puType::up;
  encoderM1.attachFullQuad(M1_ENCA_PIN, M1_ENCB_PIN); 
  encoderM2.attachFullQuad(M2_ENCA_PIN, M2_ENCB_PIN);
  encoderM1.clearCount(); encoderM2.clearCount();
}

void configurerProfilMoteur(int motorId, int currentDir, int speedPct, String keyword, float value) {
  if (motorId == 1) {
    requestedSpeedM1 = speedPct * currentDir;
    if (keyword == "tour" || keyword == "tours") {
      isTimeModeM1 = false; profileDtotalM1 = (long)(value * ticksPerRevolution);
      startTicksM1 = encoderM1.getCount() * M1_ENCODER_DIR;
      profileDaccelM1 = 0; profilePhaseM1 = 1; targetSpeedM1 = requestedSpeedM1;
      isPositionModeM1 = true;
    } else if (keyword == "sec" || keyword == "secondes") {
      isPositionModeM1 = false; long totalMs = value * 1000; long timeNeeded = abs(speedPct) * RAMP_INTERVAL; 
      if (totalMs >= 2 * timeNeeded) { timeVmaxM1 = abs(speedPct); timeTdecelM1 = totalMs - timeNeeded; } 
      else { timeVmaxM1 = totalMs / (2 * RAMP_INTERVAL); timeTdecelM1 = totalMs / 2; }
      timeDurationM1 = totalMs; timeStartM1 = millis(); targetSpeedM1 = timeVmaxM1 * currentDir; isTimeModeM1 = true;
    } else { isPositionModeM1 = false; isTimeModeM1 = false; targetSpeedM1 = requestedSpeedM1; }
  } else if (motorId == 2) {
    requestedSpeedM2 = speedPct * currentDir;
    if (keyword == "tour" || keyword == "tours") {
      isTimeModeM2 = false; profileDtotalM2 = (long)(value * ticksPerRevolution);
      startTicksM2 = encoderM2.getCount() * M2_ENCODER_DIR;
      profileDaccelM2 = 0; profilePhaseM2 = 1; targetSpeedM2 = requestedSpeedM2;
      isPositionModeM2 = true;
    } else if (keyword == "sec" || keyword == "secondes") {
      isPositionModeM2 = false; long totalMs = value * 1000; long timeNeeded = abs(speedPct) * RAMP_INTERVAL;
      if (totalMs >= 2 * timeNeeded) { timeVmaxM2 = abs(speedPct); timeTdecelM2 = totalMs - timeNeeded; } 
      else { timeVmaxM2 = totalMs / (2 * RAMP_INTERVAL); timeTdecelM2 = totalMs / 2; }
      timeDurationM2 = totalMs; timeStartM2 = millis(); targetSpeedM2 = timeVmaxM2 * currentDir; isTimeModeM2 = true;
    } else { isPositionModeM2 = false; isTimeModeM2 = false; targetSpeedM2 = requestedSpeedM2; }
  }
}

void traiterCommande(String input) {
  input.trim();
  if (input.length() > 0) {
    int motorId; char dirChar; int speedPct; char keyword[10] = ""; float value = 0;
    if (sscanf(input.c_str(), "%d %c %d %9s %f", &motorId, &dirChar, &speedPct, keyword, &value) >= 3) {
      speedPct = constrain(speedPct, 0, 100);
      int currentDir = (dirChar == 'A' || dirChar == 'a') ? 1 : -1;
      String kw = String(keyword); kw.toLowerCase();
      if (motorId == 0) { configurerProfilMoteur(1, currentDir, speedPct, kw, value); configurerProfilMoteur(2, currentDir, speedPct, kw, value); } 
      else { configurerProfilMoteur(motorId, currentDir, speedPct, kw, value); }
    }
  }
}

void updateMotors() {
  unsigned long currentMillis = millis();
  if (isPositionModeM1) {
    long traveled = abs(encoderM1.getCount() * M1_ENCODER_DIR - startTicksM1); long remaining = profileDtotalM1 - traveled;
    if (remaining <= 0 || traveled >= profileDtotalM1) { targetSpeedM1 = 0; currentSpeedM1 = 0; isPositionModeM1 = false; } 
    else {
      if (profilePhaseM1 == 1) {
        if (abs(currentSpeedM1) >= abs(requestedSpeedM1) && profileDaccelM1 == 0) profileDaccelM1 = traveled;
        if (profileDaccelM1 > 0) { if (remaining <= profileDaccelM1) { profilePhaseM1 = 2; profileDdecelM1 = remaining; profileVmaxM1 = abs(currentSpeedM1); } } 
        else { if (remaining <= traveled) { profilePhaseM1 = 2; profileDdecelM1 = remaining; profileVmaxM1 = abs(currentSpeedM1); } }
      }
      if (profilePhaseM1 == 2) {
        float ratio = (float)remaining / profileDdecelM1; int safeSpeed = (int)(profileVmaxM1 * sqrt(ratio));
        if (safeSpeed < MIN_SPEED) safeSpeed = MIN_SPEED;
        currentSpeedM1 = safeSpeed * (requestedSpeedM1 >= 0 ? 1 : -1); targetSpeedM1 = currentSpeedM1;
      }
    }
  }
  if (isPositionModeM2) {
    long traveled = abs(encoderM2.getCount() * M2_ENCODER_DIR - startTicksM2); long remaining = profileDtotalM2 - traveled;
    if (remaining <= 0 || traveled >= profileDtotalM2) { targetSpeedM2 = 0; currentSpeedM2 = 0; isPositionModeM2 = false; } 
    else {
      if (profilePhaseM2 == 1) {
        if (abs(currentSpeedM2) >= abs(requestedSpeedM2) && profileDaccelM2 == 0) profileDaccelM2 = traveled;
        if (profileDaccelM2 > 0) { if (remaining <= profileDaccelM2) { profilePhaseM2 = 2; profileDdecelM2 = remaining; profileVmaxM2 = abs(currentSpeedM2); } } 
        else { if (remaining <= traveled) { profilePhaseM2 = 2; profileDdecelM2 = remaining; profileVmaxM2 = abs(currentSpeedM2); } }
      }
      if (profilePhaseM2 == 2) {
        float ratio = (float)remaining / profileDdecelM2; int safeSpeed = (int)(profileVmaxM2 * sqrt(ratio));
        if (safeSpeed < MIN_SPEED) safeSpeed = MIN_SPEED;
        currentSpeedM2 = safeSpeed * (requestedSpeedM2 >= 0 ? 1 : -1); targetSpeedM2 = currentSpeedM2;
      }
    }
  }
  if (isTimeModeM1) { long elapsed = currentMillis - timeStartM1; if (elapsed >= timeDurationM1) { targetSpeedM1 = 0; currentSpeedM1 = 0; isTimeModeM1 = false; } else if (elapsed >= timeTdecelM1) { targetSpeedM1 = 0; } }
  if (isTimeModeM2) { long elapsed = currentMillis - timeStartM2; if (elapsed >= timeDurationM2) { targetSpeedM2 = 0; currentSpeedM2 = 0; isTimeModeM2 = false; } else if (elapsed >= timeTdecelM2) { targetSpeedM2 = 0; } }

  if (currentMillis - lastRampTime >= RAMP_INTERVAL) {
    lastRampTime = currentMillis;
    if (!(isPositionModeM1 && profilePhaseM1 == 2)) { if (currentSpeedM1 < targetSpeedM1) currentSpeedM1 += RAMP_STEP; else if (currentSpeedM1 > targetSpeedM1) currentSpeedM1 -= RAMP_STEP; }
    if (!(isPositionModeM2 && profilePhaseM2 == 2)) { if (currentSpeedM2 < targetSpeedM2) currentSpeedM2 += RAMP_STEP; else if (currentSpeedM2 > targetSpeedM2) currentSpeedM2 -= RAMP_STEP; }
    updateMotorHardware(currentSpeedM1, M1_DIR_PIN, M1_PWM_PIN, M1_FORWARD_STATE);
    updateMotorHardware(currentSpeedM2, M2_DIR_PIN, M2_PWM_PIN, M2_FORWARD_STATE);
  }

  if (currentMillis - lastSpeedTime >= SPEED_INTERVAL) {
    lastSpeedTime = currentMillis;
    long ticksM1 = encoderM1.getCount() * M1_ENCODER_DIR; long ticksM2 = encoderM2.getCount() * M2_ENCODER_DIR;
    long rawSpeedM1 = ticksM1 - lastCountM1; long rawSpeedM2 = ticksM2 - lastCountM2;
    lastCountM1 = ticksM1; lastCountM2 = ticksM2;
    globalRpmM1 = (rawSpeedM1 * 20.0 * 60.0) / ticksPerRevolution; globalRpmM2 = (rawSpeedM2 * 20.0 * 60.0) / ticksPerRevolution;
  }
}