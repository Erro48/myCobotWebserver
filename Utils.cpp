#include <ArduinoJson.h>
#include "Utils.h"
#include "SPIFFS.h"

char* Utils::readFile(const char* fileName) {
  File file;
  char* fileContent;
  int counter = 0;

  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return NULL;
  }
  
  file = SPIFFS.open(fileName);
  if(!file){
      Serial.println("Failed to open file for reading");
      return NULL;
  }

  fileContent = (char*) malloc(sizeof(char) * (file.size() + 1));
  counter = 0;

  while(file.available()){
    fileContent[counter] = file.read();
    counter++;
  }
  
  fileContent[counter] = '\0';

  file.close();

  return fileContent;
}

bool Utils::writeFile(const char* fileName, const char* content) {
  File file;
  bool status;

  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return false;
  }

  file = SPIFFS.open(fileName, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return false;
  }

  status = file.print(content);
  
  file.close();

  return content;
}
