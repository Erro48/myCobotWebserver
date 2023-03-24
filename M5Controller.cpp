#include "M5Controller.h"
#include <M5Stack.h>

M5Controller::M5Controller() {}

void M5Controller::setup() {
  _cursor = { _DEFAULT_X, _DEFAULT_Y, 2, BLACK };
  M5.Lcd.begin();
  M5.Lcd.setTextSize(_cursor.size);
}

void M5Controller::print(char* string) {
  M5.Lcd.print(string);
}

void M5Controller::print(const char* string) {
  M5.Lcd.print(string);
}

void M5Controller::print(int n) {
  M5.Lcd.print(n);
}

void M5Controller::print(unsigned int n) {
  M5.Lcd.print(n);
}

void M5Controller::print(float f) {
  M5.Lcd.print(f);
}

void M5Controller::setTextSize(unsigned int value) {
  M5.Lcd.setTextSize(value);
}
