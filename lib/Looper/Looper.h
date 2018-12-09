#include "Arduino.h"
#include "LED.h"
#include "Button.h"

#ifndef LOOPER_h
#define LOOPER_h

class Looper
{
  public:
    Led led;
    int *controls;
    int looperNum;
    boolean is_pressed[NUM_CONTROLS] = {false, false, false, false};
    long press_time[NUM_CONTROLS] = {-1, -1, -1, -1};
    int long_press_time[NUM_CONTROLS] = {500, 500, 500, 500};
    Looper();
    void updateButtons(long current_time);
    Button buttons[NUM_CONTROLS];
    void init(unsigned char *ledPins, unsigned char *controlPins, unsigned char looperNum);
};

#endif