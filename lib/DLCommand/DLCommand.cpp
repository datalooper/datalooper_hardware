#include "DLCommand.h"
#include "State.h"
#include <algorithm>
#include <iterator>

DLCommand::DLCommand(){}
DLCommand::DLCommand(uint64_t command, WS2812Serial * _leds, unsigned char _buttonNum, DLObserver* _dataLooper):  leds(_leds), buttonNum(_buttonNum), dataLooper(_dataLooper)
{
    ee_storage.the_big_blob = command;
}

void DLCommand::execute(){

        switch(ee_storage.commands.action){
            case NOTE_ON:
                usbMIDI.sendNoteOn ( ee_storage.commands.data1, ee_storage.commands.data2, ee_storage.commands.data3);
                checkLed(true);
                break;
            case NOTE_OFF:
                usbMIDI.sendNoteOff ( ee_storage.commands.data1, 0, ee_storage.commands.data3);
                checkLed(false);
                break;
            case NOTE_TOGGLE:
                if(!noteToggle){
                    usbMIDI.sendNoteOn ( ee_storage.commands.data1, ee_storage.commands.data2, ee_storage.commands.data3);
                    noteToggle = true;
                } else{
                    usbMIDI.sendNoteOff ( ee_storage.commands.data1, 0, ee_storage.commands.data3);
                    noteToggle = false;
                }
                checkLed(noteToggle);
                break;
            case CC_TOGGLE:
                Serial.println("CC Toggle");
                if(!ccToggle){
                    usbMIDI.sendControlChange ( ee_storage.commands.data1, ee_storage.commands.data2, ee_storage.commands.data4);
                    ccToggle = true;
                } else{
                    usbMIDI.sendControlChange ( ee_storage.commands.data1, ee_storage.commands.data3, ee_storage.commands.data4);
                    ccToggle = false;
                }
                checkLed(ccToggle);
                break;
            case CC:
                dataLooper->clearControlChanges(ee_storage.commands.data1, ee_storage.commands.data2);
                usbMIDI.sendControlChange ( ee_storage.commands.data1, ee_storage.commands.data2, ee_storage.commands.data3);
                checkLed(true);

                break;
            case PROGRAM_CHANGE:
                usbMIDI.sendProgramChange ( ee_storage.commands.data1, 1);
                break;
            case DATALOOPER_SPECIFIC:
                checkDLCommands();
                break;
            case SYSEX:
                Serial.println("Sending sysex");
                byte array[] = {0x1E, ee_storage.commands.data1, ee_storage.commands.data2, ee_storage.commands.data3, ee_storage.commands.data4, ee_storage.commands.data5};
                usbMIDI.sendSysEx(6, array, false);
                break;
        }
}

void DLCommand::checkDLCommands(){
    switch(ee_storage.commands.data1){
        case DLACTIONS.CHANGE_MODE:
            Serial.println("changing mode");
            State::mode = ee_storage.commands.data2;          
            break;
    }
}


void DLCommand::checkLed(bool onOff){
    if(onOff){
        Serial.print("setting pixel: ");
        Serial.println(buttonNum);
        leds->setPixel(LED_NUMBERS[buttonNum], WHT.rgb);
        leds->show();
    } else{
        Serial.print("setting pixel off: ");
        Serial.println(buttonNum);
        leds->setPixel(LED_NUMBERS[buttonNum], NONE.rgb );
        leds->show();
    }
}