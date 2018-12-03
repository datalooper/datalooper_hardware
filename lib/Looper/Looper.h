#include "Arduino.h"
#define BOUNCE_WITH_PROMPT_DETECTION
#include "Bounce2.h"
#include "LED.h"

#ifndef LOOPER_h
#define LOOPER_h

class Looper
{
  public:
    Led *led;
    int *controls;
    int ccs[NUM_CONTROLS];
    int state;
    Bounce buttons[NUM_CONTROLS] = {
      Bounce (),
      Bounce (),
      Bounce (),
      Bounce ()
    };
    //button debounce time
    const int DEBOUNCE_TIME = 25;
    boolean is_pressed[NUM_CONTROLS] = {false, false, false, false};
    long press_time[NUM_CONTROLS] = {-1, -1, -1, -1};
    int long_press_time[NUM_CONTROLS] = {500, 500, 500, 500};
    Looper();
    void configureLooper(int looperNum, int instance, int ledPins[], int controlPins[]);
};

#endif