#include <Arduino.h>
#include <math.h>
#include "SerialOutController.h"

void SerialOutController::setup()
{
  Serial.begin(115200);
}

void SerialOutController::print(char* string)
{
  Serial.println(string);
}

void SerialOutController::print(int n)
{
  int strLen = int(floor(log10(n))) + 2;
  char str[strLen];
  itoa(n, str, 10);
  print(str);
}