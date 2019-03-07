#include <Arduino.h>
#include "Button.h"
#include "EEPROM.h"
#include <typeinfo>
/**
 * @file Button.cpp
 * @author Vince Cimo
 * @version 1.0
 */

Button::Button(){}

void Button::init(unsigned char control_pin, unsigned char _buttonNumber, unsigned char _ledNumber, WS2812Serial *_leds, DLObserver* _dataLooper){
     buttonNumber = _buttonNumber; //Do I have to know the button?
     led.init(_leds, _buttonNumber);
     ledNumber = _ledNumber;
     dataLooper = _dataLooper;
     pinMode(control_pin, INPUT_PULLUP);
     bounce.attach(control_pin);
     bounce.interval(DEBOUNCE_TIME);
     long_press_time = LONG_PRESS_TIME;
     long_pressed = false;
    
    loadCommands();
}

 void Button::update(unsigned long current_time){
     bounce.update();
     //On Press
     
     if(bounce.fallingEdge()){
         if (press_time != -1 &&  current_time - press_time <= DOUBLE_HIT_TIME){
             onMultiPress();
         } else{
             onPress(current_time);
         }
     }
     //On Release
     else if(bounce.risingEdge()){
         State::modeChanging = false; 
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

     if(isFastBlinking && fastBlinkTimer > 140){
         led.writeColor(NONE);
         fastBlinkTimer = 0;
     } else if(isFastBlinking && fastBlinkTimer > 50){
        led.restoreColor();
     }
 }
    
 void Button::onPress(unsigned long current_time){
     Serial.println("press");
     press_time = current_time;
     is_pressed = true;
     checkCommands(0);
 }
 void Button::onRelease(){
     Serial.println("release");
     is_pressed = false;
     long_press_time = 500;
     checkCommands(1);
 }
 void Button::onMultiPress(){
     Serial.print("multi-press");
     Serial.println();
     press_time = -1;
     if(!checkCommands(2)){
         onPress(-1);
     }
 }
 void Button::onMultiRelease(){
     Serial.println("multi-release");
     is_pressed = false;
     long_press_time = 500;
     if(!checkCommands(3)){
         onRelease();
     }
 }
 void Button::onLongPress(){
     Serial.println("long-press");
     if(long_press_time == 500){
        checkCommands(4);
     }
     long_press_time += 500;
     long_pressed = true;
     
 }
 void Button::onLongRelease(){
     Serial.println("long-release");

     is_pressed = false;
     long_pressed = false;
     long_press_time = 500;
     checkCommands(5);
 }



 bool Button::checkCommands(unsigned char execute_on){
  
        //Iterate through all commands loaded from EEPROM. Only 1 bank of commands loaded at a time to save runtime memory
        bool exec = false;
        for (int x=0; x < NUMBER_COMMANDS; x++){
            //Check if button action matches command action
            if(commands[x].ee_storage.commands.button_action == execute_on){

                //If current mode is NEW_SESSION_MODE, modify looper record commands for unquantized recording
                if(State::mode == MODES.NEW_SESSION_MODE && isLooperRecordButton()){
                    Serial.println("unquantized recording");
                    DLCommand tempRec = commands[x];
                    //Changing record to unquantized
                    tempRec.ee_storage.commands.data3 = 1;
                    tempRec.execute();
                    exec = true;
                } 
                //If in another mode (right now, just user mode), execute the command normally.
                else if(State::mode == commands[x].ee_storage.commands.mode || (commands[x].ee_storage.commands.mode == MODES.ALL_BUT_USER && State::mode != MODES.USER_MODE)) {
                    commands[x].execute();
                    exec = true;
                }
                    
            }
        }

        return exec;

 }
void Button::diagnoseCommand(unsigned char execute_on, unsigned char x){
    Serial.print("mode: ");
    Serial.print((unsigned char) commands[x].ee_storage.commands.mode);
    Serial.print(" : ");
    Serial.println(State::mode);
    Serial.println();
    Serial.print("action: ");
    Serial.print((unsigned char) commands[x].ee_storage.commands.button_action );
    Serial.print(" : ");
    Serial.println(execute_on);
}
void Button::addCommand(ee_storage_typ command, unsigned char commandNum){
    commands[commandNum] = DLCommand(command.the_big_blob, &led, buttonNumber, dataLooper);
}
 void Button::loadCommands(){
     shouldBlink = false;
     for(unsigned char x = 0; x < NUMBER_COMMANDS; x++){
         if(x < NUMBER_USER_COMMANDS){
            unsigned char startByte = (buttonNumber * NUMBER_USER_COMMANDS * BYTES_PER_COMMAND) + (x * BYTES_PER_COMMAND);
            ee_storage_typ cmdBytes;

            for(unsigned char n = 0; n < BYTES_PER_COMMAND; n++){
                cmdBytes.asBytes[n] = EEPROM.read(startByte + n);
            }
                commands[x] = DLCommand(cmdBytes.the_big_blob, &led, buttonNumber, dataLooper);
                if(shouldButtonBlink(commands[x])){
                    shouldBlink = true;
                } 
        } else{
                ee_storage_typ dummyCmd;
                dummyCmd.commands.button_action = 127;
                dummyCmd.commands.action = 127;
                commands[x] = DLCommand(dummyCmd.the_big_blob, &led, 127, dataLooper);
        }
     }
     
 }

 bool Button::shouldButtonBlink(DLCommand command){
     if( command.ee_storage.commands.action == SYSEX && command.ee_storage.commands.data1 == 1 && command.ee_storage.commands.data3 == 0 && command.ee_storage.commands.mode == State::mode){
            return true;
        }
    return false;
 }

 void Button::toggleBlink(unsigned char blinkType){
     switch(blinkType){
         case 0:
            shouldBlink = false;
            fastBlink(false);
            break;
        case 1:
            shouldBlink = true;
            fastBlink(false);
            break;
        case 2:
            shouldBlink = false;
            fastBlink(true);
            break;
     }
 }
 void Button::requestState(){

     for(unsigned char x =0; x< NUMBER_COMMANDS; x++){
         commands[x].requestState();
     }
 }
 bool Button::isLooperRecordButton(){
     for(int x=0; x<NUMBER_USER_COMMANDS; x++){
        if( commands[x].ee_storage.commands.action == SYSEX && commands[x].ee_storage.commands.data1 == 1 && commands[x].ee_storage.commands.data3 == 0 && commands[x].ee_storage.commands.mode == MODES.USER_MODE){
            return true;
        }
     }
     return false;
 }
 
 void Button::clearControlChanges(unsigned char _ccNum, unsigned char _ccValue){
     for(unsigned char x = 0; x < NUMBER_USER_COMMANDS; x++){
         if(commands[x].ee_storage.commands.action == CC && commands[x].ee_storage.commands.data1 == _ccNum && commands[x].ee_storage.commands.data2 != _ccValue && commands[x].ee_storage.commands.led_control) {
             led.setColor(NONE);
         }
     }
 }

void Button::onControlChange(uint8_t channel, uint8_t control, uint8_t value){
    for(int x=0; x<NUMBER_USER_COMMANDS; x++){
        if( commands[x].ee_storage.commands.action == CC_TOGGLE && commands[x].ee_storage.commands.data1 == control && value > 0 && commands[x].ee_storage.commands.data4 == channel){
            led.setColor(WHT);
        } else if(commands[x].ee_storage.commands.action == CC_TOGGLE && commands[x].ee_storage.commands.data1 == control &&  value == 0 && commands[x].ee_storage.commands.data4 == channel){
            led.setColor(NONE);
        }
    }
}
void Button::onBeat(unsigned char beatNum){
    // Serial.print("button #");
    // Serial.print(buttonNumber);
    // Serial.print(" Shouldblink:");
    // Serial.println(shouldBlink);
    if(shouldBlink){
        led.writeColor(NONE);            
        return;               
    }
    
}
void Button::unBlink(){
    if(shouldBlink){
        //Serial.println("restoring");
        led.restoreColor();
        return;               
    }
}

void Button::fastBlink(bool shouldFastBlink){
    if(shouldFastBlink){
        Serial.print("fast blinking #");
        Serial.println(buttonNumber);
        isFastBlinking = true;
        fastBlinkTimer = 0;
    } else{
        Serial.print("fast blinking off #");
        Serial.println(buttonNumber);
        isFastBlinking = false;
        led.restoreColor();
    }
}

void Button::changeMode(){
    led.setColor(NONE);
    switch(State::mode){
        case MODES.USER_MODE:
            Serial.println("User Mode");
            if(isLooperRecordButton()){
                shouldBlink = true;
            }
            led.restoreColor();
        break;
        case MODES.NEW_SESSION_MODE:
            if(isLooperRecordButton()) {
                Serial.println("New Session Mode");
                led.setColor(PURPLE);
                shouldBlink = false;
            }
        break;
        case MODES.CLIP_LAUNCH_MODE:
            
        break;
    }
}
bool Button::onLooperStateChange(unsigned char _looperNum){
    for(unsigned char x = 0; x < NUMBER_USER_COMMANDS; x++){
         if(commands[x].ee_storage.commands.action == SYSEX && commands[x].ee_storage.commands.data1 == 1 && commands[x].ee_storage.commands.data2 == _looperNum+1 && commands[x].ee_storage.commands.data3 == 0) {
             return true;
         }
         return false;
     }
}

void Button::updateLooperState(unsigned char _looperState){
    switch(_looperState){
                 case looper_state.RECORDING:
                    led.setColor(RED);
                    break;
                 case looper_state.STOPPED:
                    led.setColor(BLUE);
                    break;
                 case looper_state.OVERDUBBING:
                    led.setColor(YELLOW);
                    break;
                 case looper_state.CLEAR:
                    if(State::mode == MODES.USER_MODE){
                        led.setColor(WHT);
                    } else if(State::mode == MODES.NEW_SESSION_MODE){
                        led.setColor(PURPLE);
                    }
                    break;
                case looper_state.PLAYING:
                    led.setColor(GREEN);
                    if(State::mode == MODES.NEW_SESSION_MODE){
                        State::mode = MODES.USER_MODE;
                        Serial.println("exiting new session mode");
                    }
                    break;
                case looper_state.OFF:
                    led.setColor(NONE);
                    break;
                 default:
                    break;
             }
}