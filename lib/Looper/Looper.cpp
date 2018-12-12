#include "Looper.h"

Looper::Looper() {
}
void Looper::init(unsigned char *ledPins, unsigned char *controlPins, unsigned char looperNum){
    this->looperNum = looperNum;
    state = STATE_CLEAR;
    led.init(ledPins);
    for(int x=0; x<NUM_CONTROLS; x++){
        buttons[x].init(controlPins, x, looperNum, &state);
    }
}   
void Looper::updateButtons(long current_time){
    //Row 3 of buttons is backwards on prototypes
    if ( looperNum == NUM_LOOPERS-1){
       for (int n = 0; n < NUM_CONTROLS; n++) {
            buttons[n].update(true, current_time);
       }
    }  else{
        for (int n = 0; n < NUM_CONTROLS; n++) {
            buttons[n].update(false,current_time);
        } 
    }
}

void Looper::updateState(unsigned char _state){
    led.setColor(_state);
    state = _state;
    Serial.print("updating state to: ");
    Serial.print(state);
    Serial.println();
}    


      