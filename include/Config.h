#pragma once
#include "secrets.h" 

const char* const WIFI_SSID = SECRET_WIFI_SSID;
const char* const WIFI_PASSWORD = SECRET_WIFI_PASS;
const char* const OTA_PASSWORD = SECRET_OTA_PASS;

#define M1_DIR_PIN 4
#define M1_PWM_PIN 5
#define M2_DIR_PIN 6
#define M2_PWM_PIN 7

#define M1_ENCA_PIN 15
#define M1_ENCB_PIN 16
#define M2_ENCA_PIN 17
#define M2_ENCB_PIN 18

const bool M1_FORWARD_STATE = true; 
const bool M2_FORWARD_STATE = false; 
const int M1_ENCODER_DIR = 1;  
const int M2_ENCODER_DIR = -1; 

const int RAMP_STEP = 1;                
const unsigned long RAMP_INTERVAL = 20; 
const int MIN_SPEED = 5;