#include <Arduino.h>
#include "dlconst.h"
#ifndef LED
#define LED

class Led
{
  public:
    int ledPins[LED_PINS];
    int curColor;
    Led();
    void init(int*ledPins);
    void writeColor(int color);
    void setColor(int color);
    void writeColor(int red, int green, int blue);
    void restoreColor();
};

#endif