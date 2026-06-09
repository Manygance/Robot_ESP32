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

  ArduinoOTA.setPassword(OTA_PASSWORD);
  ArduinoOTA.begin();

  server.on("/", []() { server.send(200, "text/html", htmlPage); });
  server.on("/cmd", []() { if (server.hasArg("c")) { traiterCommande(server.arg("c")); server.send(200, "text/plain", "OK"); } });
  server.on("/tune", []() { if (server.hasArg("val")) { setTicksPerRevolution(server.arg("val").toFloat()); server.send(200, "text/plain", "OK"); } });
  server.on("/status", []() {
    String json = "{\"rpm1\":" + String(getRpmM1(), 1) + ", \"rpm2\":" + String(getRpmM2(), 1) + ", \"ticks\":" + String(getTicksPerRevolution(), 1) + "}";
    server.send(200, "application/json", json);
  });
  server.begin();
  xTaskCreatePinnedToCore(WebTaskCode, "WebTask", 10000, NULL, 1, &WebTask, 0);
}