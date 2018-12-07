#include "Looper.h"

Looper::Looper(){

}
Looper::Looper(int *ledPins, int *controlPins, int looperNum) {
    //Led led;
    //led.init(ledPins);
}
void Looper::updateButtons(long current_time){
    //Row 3 of buttons is backwards on prototypes
    if ( *looperNum == NUM_LOOPERS-1){
       for (int n = 0; n < NUM_CONTROLS; n++) {
            buttons[n].update(true, current_time);
       }
    }  else{
        for (int n = 0; n < NUM_CONTROLS; n++) {
            buttons[n].update(false,current_time);
        } 
    }
}
    


      