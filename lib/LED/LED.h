#include <Arduino.h>
#include "dlconst.h"
#ifndef LED
#define LED

class Led
{
  public:
    unsigned char pins[LED_PINS];
    unsigned char curColor;
    Led();
    void init(unsigned char*ledPins);
    void writeColor(unsigned char color);
    void setColor(unsigned char color);
    void writeColor(unsigned char red, unsigned char green, unsigned char blue);
    void restoreColor();
};

#endif