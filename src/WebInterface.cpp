#include "WebInterface.h"
#include "Config.h"
#include "MotorControl.h"
#include "WebPage.h"
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoOTA.h>

WebServer server(80);
TaskHandle_t WebTask;

void WebTaskCode(void * pvParameters) {
  for(;;) { server.handleClient(); ArduinoOTA.handle(); vTaskDelay(10 / portTICK_PERIOD_MS); }
}

void initWebInterface() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) { delay(500); }
  Serial.println("\nWi-Fi connecté ! IP : "); Serial.println(WiFi.localIP());

  ArduinoOTA.setHostname("Robot-ESP32-Bastien");
  ArduinoOTA.setPassword(OTA_PASSWORD);
  ArduinoOTA.begin();

  server.on("/", []() { server.send(200, "text/html", htmlPage); });
  server.on("/cmd", []() { if (server.hasArg("c")) { traiterCommande(server.arg("c")); server.send(200, "text/plain", "OK"); } });
  server.on("/tune", []() { if (server.hasArg("val")) { setTicksPerRevolution(server.arg("val").toFloat()); server.send(200, "text/plain", "OK"); } });
  server.on("/status", []() {
    String json = "{\"speed1\": " + String(getSpeedM1()) + 
              ", \"speed2\": " + String(getSpeedM2()) + 
              ", \"pwm1\": " + String(abs(getPwmM1()) * 100 / 255) + 
              ", \"pwm2\": " + String(abs(getPwmM2()) * 100 / 255) + "}";server.send(200, "application/json", json);
  });
  server.on("/accel", []() { if (server.hasArg("val")) { setAcceleration(server.arg("val").toFloat()); server.send(200, "text/plain", "OK"); } });
  server.begin();
  xTaskCreatePinnedToCore(WebTaskCode, "WebTask", 10000, NULL, 1, &WebTask, 0);
}