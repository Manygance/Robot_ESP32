#pragma once
#include <Arduino.h>

// --- PHYSIQUE DU ROBOT ---
const float WHEEL_DIAMETER_MM = 73.0;
const float WHEEL_CIRCUMFERENCE_MM = 3.1415926535 * WHEEL_DIAMETER_MM;
const float TICKS_PER_MM = 340.0 / WHEEL_CIRCUMFERENCE_MM;

const float MAX_SPEED_MMS = 1500.0; 

// --- FONCTIONS PRINCIPALES ---
void initMotors();
void updateMotors();
void traiterCommande(String input);
void setAcceleration(float accel);

// --- LECTURE DES DONNÉES ---
float getSpeedM1(); 
float getSpeedM2(); 
int getPwmM1();     
int getPwmM2();     
float getTicksPerRevolution();
void setTicksPerRevolution(float val);

// --- COMMANDES PID ---
void setVelocity(int motorId, float speed_mms);
void moveDistance(int motorId, float distance_mm, float speed_mms);
bool isMovementFinished();