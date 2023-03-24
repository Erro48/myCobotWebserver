#include <ESPmDNS.h>
#include <Arduino.h>
#include "MyCobotWebServer.h"
#include "JsonMapper.h"
#include "IMyCobotController.h"
#include "MyCobotController.h"

MyCobotWebServer* server;

void setup() {
  Serial.begin(115200);
  server = new MyCobotWebServer(80);
  WebServerErrors connectionStatus = server->start();

  if (connectionStatus == WebServerErrors::WifiConnectionFail) {
    Serial.println("WiFi Connect Failed! Rebooting...");
    delay(1000);
    ESP.restart();
  }
}

void loop() {}
