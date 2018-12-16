#include <Arduino.h>
#include "Button.h"
#include "EEPROM.h"
#include <typeinfo>
/**
 * @file Button.cpp
 * @author Vince Cimo
 * @version 1.0
 */

//wbdavis29@yahoo.com

Button::Button() {
}

void Button::init(unsigned char *control_pin, unsigned char buttonNum, unsigned char looperNum, unsigned char *_state){
    unsigned char pin  = *(control_pin+buttonNum);
    state = _state;
    buttonNumber = buttonNum;
    pinMode(pin, INPUT_PULLUP);
    bounce.attach(pin);
    bounce.interval(DEBOUNCE_TIME);
    long_press_time = LONG_PRESS_TIME;
    long_pressed = false;
    
    //loadCommands();
}

void Button::update(bool isFlipped, unsigned long current_time){
    bounce.update();

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
        } else if(long_pressed){
            onLongRelease();
        } else{
            onRelease();
        }
    }
    
    //checks for a long press
    if(is_pressed && current_time - press_time >= long_press_time){
        onLongPress();
    }
}
    
void Button::onPress(unsigned long current_time){
    Serial.print("press");
    Serial.println();
    press_time = current_time;
    is_pressed = true; 
    checkCommands(0, 4);
}
void Button::onRelease(){
    Serial.print("release");
    Serial.println();
    is_pressed = false;
    long_press_time = 500;
    checkCommands(5, 9);
}
void Button::onMultiPress(){
    Serial.print("multi-press");
    Serial.println();
    press_time = -1;
    checkCommands(10, 14);
}
void Button::onMultiRelease(){
    Serial.print("multi-release");
    Serial.println();
    is_pressed = false;
    long_press_time = 500;
    checkCommands(15, 19);
}
void Button::onLongPress(){
    long_press_time += 500;
    long_pressed = true;
    checkCommands(20, 24);
}
void Button::onLongRelease(){
    is_pressed = false;
    long_pressed = false;
    long_press_time = 500;
    checkCommands(25, 29);
}
void Button::checkCommands(unsigned char low, unsigned char high){

    for (int x=0; x < NUMBER_COMMANDS; x++){
        if(commands[x].execute_on >= low && commands[x].execute_on <= high){
            commands[x].execute();
        }
    }
}

void Button::loadCommands(){
    // 3 bytes per command , 3 commands
    // button 1: bytes 0-2 command 1, 3-5 commmand 2, 6-8 command 3
    // button 2: bytes 9-11 command 1, etc..
    // buttonNum (zeroindex) * 9
    // unsigned char startByte = buttonNumber * NUMBER_COMMANDS * BYTES_PER_COMMAND;
    // for(unsigned char n = 0; n < NUMBER_COMMANDS; n++){
    //     unsigned char execute_on = EEPROM.read(startByte);
    //     unsigned char action = EEPROM.read(startByte+1);
    //     unsigned char data1 = EEPROM.read(startByte+2);
    //     unsigned char data2 = EEPROM.read(startByte+2);
    //     if(execute_on != 255 && action != 255 && data1 != 255 && data2 != 255){
    //         commands.push_back(DLCommand(execute_on, 0, action,data1, data2));
    //     }
    //     startByte += BYTES_PER_COMMAND;
    // }
}
