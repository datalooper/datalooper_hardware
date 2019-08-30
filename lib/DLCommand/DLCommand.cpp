#include "DLCommand.h"
#include "State.h"
#include <algorithm>
#include <iterator>

DLCommand::DLCommand()
{
    ee_storage.commands.action = 127;
    ee_storage.commands.button_action = 127;
    ee_storage.commands.mode = 127;
}
DLCommand::DLCommand(uint64_t command, DLled * _led, unsigned char _buttonNum, DLObserver* _dataLooper):  led(_led), dataLooper(_dataLooper)
{
    ee_storage.the_big_blob = command;
}

void DLCommand::execute(){
    if(State::inConfig == 0){
        Serial.println("executing command");
        Serial.println((unsigned char) ee_storage.commands.button_action);
        switch(ee_storage.commands.action){
            case NOTE_ON:
                Serial.println("sending note on");
                usbMIDI.sendNoteOn ( ee_storage.commands.data1, ee_storage.commands.data2, ee_storage.commands.data3);
                dataLooper->sendNoteOn(ee_storage.commands.data1, ee_storage.commands.data2, ee_storage.commands.data3);
                checkLed(true);
                break;
            case NOTE_OFF:
                Serial.println("sending note off");
                usbMIDI.sendNoteOff ( ee_storage.commands.data1, 0, ee_storage.commands.data3);
                dataLooper->sendNoteOff(ee_storage.commands.data1, 0, ee_storage.commands.data3);
                checkLed(false);
                break;
            case NOTE_TOGGLE:
                if(!toggle){
                    Serial.println("sending note toggle on");
                    usbMIDI.sendNoteOn ( ee_storage.commands.data1, ee_storage.commands.data2, ee_storage.commands.data3);                
                    dataLooper->sendNoteOn(ee_storage.commands.data1, ee_storage.commands.data2, ee_storage.commands.data3);
                    toggle = true;
                } else{
                    Serial.println("sending note toggle off");
                    usbMIDI.sendNoteOff ( ee_storage.commands.data1, 0, ee_storage.commands.data3);
                    dataLooper->sendNoteOff(ee_storage.commands.data1, 0, ee_storage.commands.data3);
                    toggle = false;
                }
                checkLed(toggle);
                break;
            case CC_TOGGLE:
                Serial.println("CC Toggle");
                if(!toggle){
                    usbMIDI.sendControlChange ( ee_storage.commands.data1, ee_storage.commands.data2, ee_storage.commands.data4);
                    dataLooper->sendControlChange(ee_storage.commands.data1, ee_storage.commands.data2, ee_storage.commands.data4);
                    toggle = true;
                } else{
                    usbMIDI.sendControlChange ( ee_storage.commands.data1, ee_storage.commands.data3, ee_storage.commands.data4);
                    dataLooper->sendControlChange(ee_storage.commands.data1, ee_storage.commands.data2, ee_storage.commands.data4);
                    toggle = false;
                }
                checkLed(toggle);
                break;
            case CC:
                dataLooper->clearControlChanges(ee_storage.commands.data1, ee_storage.commands.data2);
                usbMIDI.sendControlChange ( ee_storage.commands.data1, ee_storage.commands.data2, ee_storage.commands.data3);
                    dataLooper->sendControlChange(ee_storage.commands.data1, ee_storage.commands.data2, ee_storage.commands.data3);
                checkLed(true);

                break;
            case PROGRAM_CHANGE:
                usbMIDI.sendProgramChange ( ee_storage.commands.data1, 1);
                dataLooper->sendProgramChange(ee_storage.commands.data1, 1);
                break;
            case SYSEX:
            {
                Serial.print("Sending sysex, mode #");
                Serial.println(State::mode);
                byte array[] = {DATALOOPER_IDENTIFIER, ee_storage.commands.data1, ee_storage.commands.data2, ee_storage.commands.data3, ee_storage.commands.data4, ee_storage.commands.data5};
                usbMIDI.sendSysEx(6, array, false);
                break;
            }
            case DATALOOPER_SPECIFIC:
                checkDLCommands();
                break;

        
        }
    }
}

void DLCommand::checkDLCommands(){
    Serial.println("checking dl commands");
    switch(ee_storage.commands.data1){
        case DLACTIONS.CHANGE_MODE:
            if(!State::modeChanging){
                Serial.print("changing mode to");
                Serial.println((unsigned char) ee_storage.commands.data2);
                State::mode = ee_storage.commands.data2;
                State::modeChanging = true;   
            }       
            return;
        case DLACTIONS.CHANGE_PRESET:
            Serial.print("changing to preset #:");
            Serial.println((unsigned char) ee_storage.commands.data2);
            State::preset = ee_storage.commands.data2;
            dataLooper->onPresetChange();
            return;    
    }
}


void DLCommand::checkLed(bool onOff){
        if(onOff){
            Serial.print("action: ");
            Serial.println((unsigned char)ee_storage.commands.action);
            Serial.print("button action: ");
            Serial.println((unsigned char)ee_storage.commands.button_action);
            Serial.print("mode: ");
            Serial.println((unsigned char)ee_storage.commands.mode);
            Serial.print("setting pixel: ");
            led->setColor(WHT);
        } else{
            Serial.print("setting pixel off: ");
            led->setColor(NONE);
        }
    
}