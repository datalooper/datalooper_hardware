#include "Looper.h"

Looper::Looper() {
}

void Looper::configureLooper(int looperNum, int instance, int ledPins[], int controlPins[]){
    led = new Led(ledPins);
    controls = controlPins;
    for(int n = 0; n < NUM_CONTROLS; n++){
        ccs[n] = (instance * NUM_LOOPERS * NUM_CONTROLS) + (looperNum * NUM_CONTROLS ) + n + 1;
        pinMode(controls[n], INPUT_PULLUP);
        buttons[n].attach(controls[n]);
        buttons[n].interval(DEBOUNCE_TIME);
      }
   }
