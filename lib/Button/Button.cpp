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
}

 void Button::update(unsigned long current_time){
     bounce.update();

     //On Press
     if(bounce.fallingEdge()){
         Serial.print("button #");
         Serial.println(buttonNumber);
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
        Serial.println("checking commands");
        //Iterate through all commands loaded from EEPROM. Only 1 bank of commands loaded at a time to save runtime memory
        bool exec = false;
        for (int x=0; x < NUMBER_COMMANDS; x++){
            //Check if button action matches command action
            if(commands[x].ee_storage.commands.button_action == execute_on && !State::modeChanging && commands[x].ee_storage.commands.action != 127 ){

                //If current mode is NEW_SESSION_MODE, modify looper record commands for unquantized recording
                if(State::mode == MODES.NEW_SESSION_MODE && isLooperCommand(commands[x], true)){
                    Serial.println("unquantized recording");
                    DLCommand tempRec = commands[x];
                    //Changing record to unquantized
                    tempRec.ee_storage.commands.data3 = 1;
                    tempRec.execute();
                    exec = true;
                } 
                //If in another mode (right now, just user mode), execute the command normally.
                else if(State::mode == commands[x].ee_storage.commands.mode || (commands[x].ee_storage.commands.mode == MODES.ALL_BUT_USER && State::mode != MODES.USER_MODE) || (State::mode == MODES.NEW_SESSION_MODE && isLooperCommand(commands[x], false)) ) {
                    // diagnoseCommand(x);
                    commands[x].execute();
                    exec = true;
                } 
                    
            }
        }

        return exec;

 }
void Button::diagnoseCommand(unsigned char x){
    Serial.print("command num: ");
    Serial.print(x);
    Serial.print(" | button action ");
    Serial.print((unsigned char) commands[x].ee_storage.commands.button_action );
    Serial.print(" | action: ");
    Serial.print((unsigned char) commands[x].ee_storage.commands.action );
    Serial.print(" | data1: ");
    Serial.println((unsigned char) commands[x].ee_storage.commands.data1 );
    Serial.print(" | data2: ");
    Serial.println((unsigned char) commands[x].ee_storage.commands.data2 );
    Serial.print(" | data3: ");
    Serial.println((unsigned char) commands[x].ee_storage.commands.data3 );
    Serial.print(" | data4: ");
    Serial.println((unsigned char) commands[x].ee_storage.commands.data4 );
    Serial.print(" | data5: ");
    Serial.println((unsigned char) commands[x].ee_storage.commands.data5 );
    Serial.print(" | led_control: ");
    Serial.println((unsigned char) commands[x].ee_storage.commands.led_control );
    Serial.print(" | mode: ");
    Serial.println((unsigned char) commands[x].ee_storage.commands.mode );
}

void Button::addCommand(ee_storage_typ command, unsigned char commandNum){
    commands[commandNum] = DLCommand(command.the_big_blob, &led, buttonNumber, dataLooper);
}
 void Button::loadCommands(){
     shouldBlink = false;
     isFastBlinking = false;
     Serial.print("loading commands on button:");
     Serial.println(buttonNumber);
     Serial.print("preset:");
     Serial.println(State::preset);
     for(unsigned char x = 0; x < NUMBER_COMMANDS; x++){
         if(x < NUMBER_USER_COMMANDS){
            int presetOffset = State::preset * NUM_BUTTONS * NUMBER_USER_COMMANDS * BYTES_PER_COMMAND; 
            if(presetOffset != 0){
                //rounds off to page size
                presetOffset += (16 * State::preset );
            }
            int startByte = (buttonNumber * NUMBER_USER_COMMANDS * BYTES_PER_COMMAND) + (x * BYTES_PER_COMMAND) + presetOffset + GLOBAL_CONFIG_PAGE_OFFSET;
            ee_storage_typ cmdBytes;

            for(unsigned char n = 0; n < BYTES_PER_COMMAND; n++){
                cmdBytes.asBytes[n] = readByte(startByte + n);
            }
                commands[x] = DLCommand(cmdBytes.the_big_blob, &led, buttonNumber, dataLooper);
                if(shouldButtonBlink(commands[x])){
                    shouldBlink = true;
                } 
                if(isCurrentPresetCommand(commands[x])){
                    led.setColor(WHT);
                }
                // diagnoseCommand(x);
        } else{
                ee_storage_typ dummyCmd;
                dummyCmd.commands.button_action = (unsigned char) 127;
                dummyCmd.commands.action = (unsigned char) 127;
                commands[x] = DLCommand(dummyCmd.the_big_blob,  &led, 127, dataLooper);
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
    //  Serial.print("requesting state on button #:");
    //  Serial.println(buttonNumber);
    //  Serial.print("current mode:");
    //  Serial.println(State::mode);
     for(unsigned char x =0; x< NUMBER_COMMANDS; x++){
        //  Serial.print("Command #");
        //  Serial.print(x);
        //  Serial.print(" action: ");
        //  Serial.println((unsigned char) commands[x].ee_storage.commands.action);
        //  Serial.print(" mode: ");
        //  Serial.println((unsigned char) commands[x].ee_storage.commands.mode);
        if(commands[x].ee_storage.commands.action == SYSEX && State::mode == commands[x].ee_storage.commands.mode){
        // Serial.println("sending state sysex");
        byte array[] = {DATALOOPER_IDENTIFIER, ACTIONS.REQUEST_STATE, buttonNumber, commands[x].ee_storage.commands.data1, commands[x].ee_storage.commands.data2, commands[x].ee_storage.commands.data3, commands[x].ee_storage.commands.data4, commands[x].ee_storage.commands.data5};
        usbMIDI.sendSysEx(8, array, false);
        }
    }
 }
 
 bool Button::isCurrentPresetCommand(DLCommand command){
     return command.ee_storage.commands.action == DATALOOPER_SPECIFIC && command.ee_storage.commands.data1 == DLACTIONS.CHANGE_PRESET && command.ee_storage.commands.data2 == State::preset;
 }
 bool Button::isLooperRecordButton(){
     for(int x = 0; x < NUMBER_USER_COMMANDS; x++){
         if(isLooperCommand(commands[x], true)){
             return true;
         }
     }
     return false;
 }
 bool Button::isLooperCommand(DLCommand command, bool isRecordCommand){
    
    if( command.ee_storage.commands.action == SYSEX && command.ee_storage.commands.data1 == 1 && command.ee_storage.commands.mode == MODES.USER_MODE){
        if(!isRecordCommand){
            return true;
        } 
        else if(isRecordCommand && (command.ee_storage.commands.data3 == 0 || command.ee_storage.commands.data3 == 1)){
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
        if( commands[x].ee_storage.commands.action == CC_TOGGLE && commands[x].ee_storage.commands.data4 == channel && commands[x].ee_storage.commands.data1 == control && value > 0 ){
            led.setColor(WHT);
            commands[x].toggle = true;
        } else if(commands[x].ee_storage.commands.action == CC_TOGGLE && commands[x].ee_storage.commands.data1 == control &&  value == 0 && commands[x].ee_storage.commands.data4 == channel){
            led.setColor(NONE);
            commands[x].toggle = false;
        } else if(commands[x].ee_storage.commands.action == CC && commands[x].ee_storage.commands.data1 == control && commands[x].ee_storage.commands.data2 == value && commands[x].ee_storage.commands.data3 == channel){
            led.setColor(WHT);
            commands[x].toggle = true;
        } else if(commands[x].ee_storage.commands.action == CC && commands[x].ee_storage.commands.data1 == control && commands[x].ee_storage.commands.data2 != value && commands[x].ee_storage.commands.data3 == channel){
            led.setColor(NONE);
            commands[x].toggle = false;
        }
    }
}
void Button::onBeat(unsigned char beatNum){
    // Serial.print("button #");
    // Serial.print(buttonNumber);
    // Serial.print(" Shouldblink:");
    // Serial.println(shouldBlink);
    if(shouldBlink && !State::inConfig){
        led.writeColor(NONE);            
        return;               
    }
    
}
void Button::unBlink(){
    if(shouldBlink && !State::inConfig){
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
    switch(State::mode){
        case MODES.USER_MODE:
            Serial.println("User Mode");
            led.setColor(NONE);
            for( int x = 0; x < NUMBER_USER_COMMANDS; x++){
                if(isLooperCommand(commands[x], true)){
                    shouldBlink = true;
                }
                if(isCurrentPresetCommand(commands[x])){
                    led.setColor(WHT);
                }
            }
        break;
        case MODES.NEW_SESSION_MODE:
            if(isLooperRecordButton() && led.curColor.rgb != NONE.rgb) {
                Serial.println("New Session Mode");
                led.setColor(PURPLE);
                shouldBlink = false;
            }
        break;
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
                    if(State::mode == MODES.NEW_SESSION_MODE){
                        State::mode = MODES.USER_MODE;
                        Serial.println("exiting new session mode");
                    }
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

void Button::startConfig(){
    currentConfigCommand = 0;
    led.setColor(RED);
}

void Button::configureDL(const uint8_t *sysExData){
    if(State::inConfig != 0){
    // Serial.println("Button number:");
    // Serial.println(buttonNumber);
          for(int byte = 0; byte < SYSEX_BYTES_PER_COMMAND; byte++){
            switch(byte){
              case 0:
                // Serial.print("Button Action: ");
                commands[currentConfigCommand].ee_storage.commands.button_action = sysExData[sysExStartByte+byte];
                break;
              case 1:
                // Serial.print("Action: ");
                commands[currentConfigCommand].ee_storage.commands.action = sysExData[sysExStartByte+byte];
                break;
              case 2:
                // Serial.print("Data 1: ");
                commands[currentConfigCommand].ee_storage.commands.data1 = sysExData[sysExStartByte+byte];
                break;
              case 3:
                // Serial.print("Data 2: ");
                commands[currentConfigCommand].ee_storage.commands.data2 = sysExData[sysExStartByte+byte];
                break;
              case 4:
                // Serial.print("Data 3: ");
                commands[currentConfigCommand].ee_storage.commands.data3 = sysExData[sysExStartByte+byte];
                break;
              case 5:
                // Serial.print("Data 4: ");
                commands[currentConfigCommand].ee_storage.commands.data4 = sysExData[sysExStartByte+byte];
                break;
              case 6:
                // Serial.print("Data 5: ");
                commands[currentConfigCommand].ee_storage.commands.data5 = sysExData[sysExStartByte+byte];
                break;  
              case 7:
                // Serial.print("LED Control: ");
                commands[currentConfigCommand].ee_storage.commands.led_control = sysExData[sysExStartByte+byte];
                break;
              case 8:
                // Serial.print("Mode: ");
                commands[currentConfigCommand].ee_storage.commands.mode = sysExData[sysExStartByte+byte];
                break;
            }
            // Serial.println (sysExData[sysExStartByte+byte]);
            
          }

    currentConfigCommand += 1;
    }
}



bool Button::shouldRequestRebuild(){
    for(int x = 0; x < NUMBER_USER_COMMANDS; x++){
        if(commands[x].ee_storage.commands.action == CC_TOGGLE || commands[x].ee_storage.commands.action == CC){
            return true;
        }
    }
    return false;
    
}

byte Button::readByte(unsigned int eeaddress ) {
    byte rdata = 0x7F;
    int daddr = 0x50;

    Wire.beginTransmission(daddr);
    Wire.write((int)(eeaddress >> 8)); // MSB
    Wire.write((int)(eeaddress & 0xFF)); // LSB
    Wire.endTransmission();
    Wire.requestFrom(daddr,1);
    if (Wire.available()) { rdata = Wire.read(); 
        Serial.print("reading byte:");
        Serial.print(rdata);
        Serial.print(" at byte addr:");
        Serial.println(eeaddress); // MSB

    }
    else{
        Serial.print("wire not available at addr:");
        Serial.println(eeaddress);
    }

    return rdata;
}

