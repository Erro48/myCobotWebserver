#ifndef __SERIAL_OUT_CONTROLLER__
#define __SERIAL_OUT_CONTROLLER__

#include "IGuiController.h"

class SerialOutController : IGuiController {
public:
  void setup() override;
  void print(char* string) override;
  void print(int n) override;
  void print(float f) override;
};

#endif