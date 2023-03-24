#ifndef __M5_CONTROLLER__
#define __M5_CONTROLLER__

#include "IGuiController.h"

class M5Controller : public IGuiController
{
    public:
      M5Controller();
      void setup() override;
      void print(char* string) override;
      void print(const char* string) override;
      void print(int n) override;
      void print(unsigned int n) override;
      void print(float f) override;

      void setTextSize(unsigned int value) override;

    private:
      typedef struct {
          int x;
          int y;
          unsigned int size;
          int color;
      } Cursor;

      Cursor _cursor;

      const int _DEFAULT_X = 50;
      const int _DEFAULT_Y = 50;

      void _updateCursor();



};

#endif