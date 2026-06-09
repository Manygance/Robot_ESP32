#pragma once
#include <Arduino.h>

void initMotors();
void updateMotors();
void traiterCommande(String input);

float getRpmM1();
float getRpmM2();
float getTicksPerRevolution();
void setTicksPerRevolution(float val);