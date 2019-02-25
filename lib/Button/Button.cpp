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
     leds = _leds;
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
     checkCommands(0);
 }
 void Button::onRelease(){
     Serial.print("release");
     Serial.println();
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
     Serial.print("multi-release");
     Serial.println();
     is_pressed = false;
     long_press_time = 500;
     if(!checkCommands(3)){
         onRelease();
     }
 }
 void Button::onLongPress(){
     Serial.print("long-press");

     long_press_time += 500;
     long_pressed = true;
     checkCommands(4);
 }
 void Button::onLongRelease(){
     Serial.print("long-release");

     is_pressed = false;
     long_pressed = false;
     long_press_time = 500;
     checkCommands(5);
 }

 bool Button::checkNewSessionCommands(){
     switch(buttonNumber){
        case EXIT_NEW_SESSION_BUTTON_NUM:
            State::mode = MODES.USER_MODE;
            break;
        case TAP_TEMPO_BUTTON_NUM:
            Serial.println("tap tempo");
            byte array[] = {0x1E, 10, 4, 0, 0, 0};
            usbMIDI.sendSysEx(6, array, false);
            break;

     }
     return false;
 }
 bool Button::checkCommands(unsigned char execute_on){
     if(State::mode == MODES.CLIP_LAUNCH_MODE){
         return false;
     } else{
        //Checks for new session mode specific commands
        if(State::mode == MODES.NEW_SESSION_MODE && execute_on == BUTTON_ACTIONS.PRESS){
            return checkNewSessionCommands();
        }
        
        //Iterate through all commands loaded from EEPROM. Only 1 bank of commands loaded at a time to save runtime memory
        for (int x=0; x < NUMBER_COMMANDS; x++){
            //diagnoseCommand(execute_on, x);

            //Check if button action matches command action
            if(commands[x].ee_storage.commands.button_action == execute_on){

                //If current mode is NEW_SESSION_MODE, modify looper record commands for unquantized recording
                if(State::mode == MODES.NEW_SESSION_MODE && isLooperRecordCommand()){
                    DLCommand tempRec = commands[x];
                    //Changing record to unquantized
                    tempRec.ee_storage.commands.data4 = 0;
                    tempRec.execute();
                } 
                //If in another mode (right now, just user mode), execute the command normally.
                else if(State::mode == commands[x].ee_storage.commands.mode) {
                    commands[x].execute();
                }
                    return true;
            }
        }
     }
     return false;

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
 void Button::loadCommands(){
     for(unsigned char x = 0; x < NUMBER_COMMANDS; x++){
         unsigned char startByte = (buttonNumber * NUMBER_COMMANDS * BYTES_PER_COMMAND) + (x * BYTES_PER_COMMAND);
         ee_storage_typ cmdBytes;

		 for(unsigned char n = 0; n < BYTES_PER_COMMAND; n++){
			 cmdBytes.asBytes[n] = EEPROM.read(startByte + n);
		 }
			 commands[x] = DLCommand(cmdBytes.the_big_blob, leds, buttonNumber, dataLooper);
    }
 }
 bool Button::isLooperRecordCommand(){
     for(int x=0; x<NUMBER_COMMANDS; x++){
        if( commands[x].ee_storage.commands.action == SYSEX && commands[x].ee_storage.commands.data1 == 1 && commands[x].ee_storage.commands.data3 == 0){
            return true;
        }
     }
     return false;
 }
 void Button::clearControlChanges(unsigned char _ccNum, unsigned char _ccValue){
     for(unsigned char x = 0; x < NUMBER_COMMANDS; x++){
         if(commands[x].ee_storage.commands.action == CC && commands[x].ee_storage.commands.data1 == _ccNum && commands[x].ee_storage.commands.data2 != _ccValue && commands[x].ee_storage.commands.led_control) {
             setColor(NONE);
         }
     }
 }
 void Button::setColor(colorType color){
     curColor = color;
     if(color.rgb != NONE.rgb){
        Serial.print("red: ");
        Serial.println(curColor.colors.red);
        Serial.print("blue: ");
        Serial.println(curColor.colors.blue);
        Serial.print("green: ");
        Serial.println(curColor.colors.green);
    }
    writeColor(curColor);
 }

 void Button::restoreColor(){
     writeColor(curColor);
 }
void Button::writeColor(colorType color){
     leds->setPixel(ledNumber, color.rgb);
     leds->show();
}
void Button::onBeat(unsigned char beatNum){
    if(isLooperRecordCommand() && State::mode == MODES.USER_MODE){
        //Serial.println("on beat");
        writeColor(NONE);            
        return;               
    }
    
}
void Button::unBlink(){
    if(isLooperRecordCommand()){
        //Serial.println("restoring");
        restoreColor();
        return;               
    }

}

void Button::changeMode(){
    switch(State::mode){
        case MODES.USER_MODE:
            if(curColor.rgb == PURPLE.rgb && isLooperRecordCommand()){
                setColor(WHT);
            }
        break;
        case MODES.NEW_SESSION_MODE:
            Serial.println("New Session Mode");
            if(isLooperRecordCommand()) {
                setColor(PURPLE);
            }
        break;
        case MODES.CLIP_LAUNCH_MODE:

        break;
    }
}
void Button::onLooperStateChange(unsigned char _looperNum, unsigned char _looperState){
    // Serial.println("in state change");
    // Serial.print("looper num: ");
    // Serial.println(_looperNum);
    // Serial.print("button number: ");
    // Serial.println(buttonNumber);

    for(unsigned char x = 0; x < NUMBER_COMMANDS; x++){
            // Serial.print("action: ");
            // Serial.println((unsigned char) commands[x].ee_storage.commands.action);
            // Serial.print("data1: ");
            // Serial.println((unsigned char) commands[x].ee_storage.commands.data1);
            // Serial.print("data2: ");
            // Serial.println((unsigned char) commands[x].ee_storage.commands.data2);
         if(commands[x].ee_storage.commands.action == SYSEX && commands[x].ee_storage.commands.data1 == 1 && commands[x].ee_storage.commands.data2 == _looperNum+1 && commands[x].ee_storage.commands.data3 == 0) {
             switch(_looperState){
                 case looper_state.RECORDING:
                    setColor(RED);
                    break;
                 case looper_state.STOPPED:
                    setColor(BLUE);
                    break;
                 case looper_state.OVERDUBBING:
                    setColor(YELLOW);
                    break;
                 case looper_state.CLEAR:
                    if(State::mode == MODES.USER_MODE){
                        setColor(WHT);
                    } else if(State::mode == MODES.NEW_SESSION_MODE){
                        setColor(PURPLE);
                    }
                    break;
                case looper_state.PLAYING:
                    setColor(GREEN);
                    if(State::mode == MODES.NEW_SESSION_MODE){
                        State::mode = MODES.USER_MODE;
                        Serial.println("exiting new session mode");
                    }
                    break;
                 default:
                    break;
             }
         }
     }
}
