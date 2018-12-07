#include <Arduino.h>
#include "Button.h"
#include "EEPROM.h"

/**
 * @file Button.cpp
 * @author Vince Cimo
 * @version 1.0
 */

Button::Button() {

}

void Button::init(int control_pin, int buttonNum){
    bounce = Bounce ();
    pinMode(control_pin, INPUT_PULLUP);
    bounce.attach(control_pin);
    bounce.interval(DEBOUNCE_TIME);
}

void Button::update(boolean isFlipped, long current_time){
    //TODO REMOVE THIS ON FINAL VERSION

    //On Press
    if((bounce.risingEdge() && !isFlipped) || (bounce.fallingEdge() && isFlipped)){
        if (press_time != -1 &&  current_time - press_time <= DOUBLE_HIT_TIME){
            onMultiPress();
        } else{
            onPress(current_time);
        }
    } 
    //On Release
    else if((bounce.fallingEdge() && !isFlipped) || (bounce.risingEdge() && isFlipped)){
        if (press_time == -1){
            onMultiRelease();
        } else{
            onRelease();
        }
    }
    
    //checks for a long press
    if(is_pressed && current_time - press_time >= long_press_time){
        onLongPress();
    }
}
    

    
void Button::onPress(long current_time){
    press_time = current_time;
    is_pressed = true; 
    checkCommands(0, 4);
}
void Button::onRelease(){
    is_pressed = false;
    long_press_time = 500;
    checkCommands(5, 9);
}
void Button::onMultiPress(){
  press_time = -1;
  //sendSysEx(looper, control, MULTI_TAP, 0);
  checkCommands(10, 14);
}
void Button::onMultiRelease(){
  //sendSysEx(looper, control, MULTI_RELEASE, 0);
  is_pressed = false;
  long_press_time = 500;
  checkCommands(15, 19);
}
void Button::onLongPress(){
    long_press_time += 500;
    checkCommands(20, 127);
}

void Button::checkCommands(int low, int high){
    for(Command &command : commands){
        if (command.dataType <= high && command.dataType >= low){
            command.execute();
        }
    }
}

// void Button::diagnoseButton(int i, int n, int num){
//   Serial.print("looper:");
//   Serial.print( i );
//   Serial.println();
//   Serial.print("button: ");
//   Serial.print(n);
//   Serial.println();
//   Serial.print("cc#: ");
//   Serial.print(num);
//   Serial.println();
// }

void Button::loadCommands(){
    // 3 bytes per command , 3 commands
    // button 1: bytes 0-2 command 1, 3-5 commmand 2, 6-8 command 3
    // button 2: bytes 9-11 command 1, etc..
    // buttonNum (zeroindex) * 9
    int startByte = buttonNumber * NUMBER_COMMANDS * BYTES_PER_COMMAND;
    int endByte = startByte + NUMBER_COMMANDS * BYTES_PER_COMMAND;
    // for(int n = 0; n < NUMBER_COMMANDS; n++){
    //     for(int x = 0; x < BYTES_PER_COMMAND; x++){
    //         EEPROM.read(startByte+x)
    //     }
    //     startByte += BYTES_PER_COMMAND;
    // }
}
