#include <Arduino.h>
#include <WiFi.h>
#include "WifiConnection.h"
#include "SPIFFS.h"
#include "Utils.h"

WifiConnection::WifiConnection() {
  char* fileContent;

  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  fileContent = Utils::readFile("/network.json");
  if (fileContent != NULL) {
    _saveWifiConfig(fileContent);
  } else {
    Serial.println("Cannot read file");
  }
}

char** WifiConnection::discover() {
  int networksNumber = WiFi.scanNetworks();

  char *networks[networksNumber];
  for (unsigned int i = 0; i < networksNumber; i++)
  {
    const char* ssid = WiFi.SSID(i).c_str();
    int len = strlen(ssid) + 1;
    Serial.print("Lunghezza ssid: ");
    Serial.println(len);
    
    networks[i] = new char[len];
    strcpy(networks[i], ssid);

    Serial.print(i);
    Serial.print(". ");
    Serial.print(networks[i]);
    Serial.print(" (");
    Serial.print(strlen(networks[i]));
    Serial.println(")");
  }
  return networks;
}

bool WifiConnection::connect() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(_ssid, _password);

  return WiFi.waitForConnectResult() == WL_CONNECTED;
}

IPAddress WifiConnection::ip() {
  IPAddress ip = WiFi.localIP();
  return ip;
}

char* WifiConnection::network() {
  return _ssid;
}

bool WifiConnection::_storeData(char* key, char* arg) {
  if (strcmp(key, "ssid") == 0) {
    _ssid = (char*) malloc(sizeof(char) * strlen(arg));
    strcpy(_ssid, arg);
    return true;
  }
  
  if (strcmp(key, "password") == 0) {
    _password = (char*) malloc(sizeof(char) * strlen(arg));
    strcpy(_password, arg);
    return true;
  }

  return false;
}

void WifiConnection::_saveWifiConfig(char* fileContent) {
  StaticJsonDocument<32> doc;
  deserializeJson(doc, fileContent);

  if (doc["ssid"] != NULL && doc["password"] != NULL) {

    int ssidLen = strlen(doc["ssid"].as<const char*>());
    int passwordLen = strlen(doc["password"].as<const char*>());
    char* ssid = (char*) malloc(sizeof(char) * ssidLen);
    char* password = (char*) malloc(sizeof(char) * passwordLen);

    strcpy(ssid, doc["ssid"].as<const char*>());
    strcpy(password, doc["password"].as<const char*>());

    if (!_storeData("ssid", ssid) || !_storeData("password", password)) {
      Serial.println("Wrong config parameters");
      return;
    }
  }
}