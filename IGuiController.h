#ifndef __IGUI_CONTROLLER__
#define __IGUI_CONTROLLER__

class IGuiController {
  public:
    /**
     * Setup function.
    */
    virtual void setup();
    
    /**
     * Prints the given char array.
     * @param string text to print.
    */
    virtual void print(char* string) = 0;

    /**
     * Prints the given constant char array.
     * @param string text to print.
    */
    virtual void print(const char* string) = 0;

    /**
     * Prints the given integer.
     * @param n number to print.
    */
    virtual void print(int n) = 0;

    /**
     * Prints the given unsigned integer.
     * @param n number to print.
    */
    virtual void print(unsigned int n) = 0;

    /**
     * Prints the given float.
     * @param f number to print.
    */
    virtual void print(float f) = 0;

    /**
     * Sets the text size of the font.
     * @param value of the new text size.
    */
    virtual void setTextSize(unsigned int value) = 0;
};

#endif