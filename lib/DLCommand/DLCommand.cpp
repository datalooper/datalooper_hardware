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
DLCommand::DLCommand(uint64_t command, DLled * _led, unsigned char _buttonNum, DLObserver* _dataLooper):  led(_led), buttonNum(_buttonNum), dataLooper(_dataLooper)
{
    ee_storage.the_big_blob = command;
}
void DLCommand::requestState(){
    if(ee_storage.commands.action == SYSEX && buttonNum <= NUM_BUTTONS && State::mode == ee_storage.commands.mode){
        byte array[] = {0x1E, ACTIONS.REQUEST_STATE, buttonNum, ee_storage.commands.data1, ee_storage.commands.data2, ee_storage.commands.data3, ee_storage.commands.data4, ee_storage.commands.data5};
        usbMIDI.sendSysEx(8, array, false);
    }
}
void DLCommand::execute(){
        // Serial.println("executing command");
        // Serial.println((unsigned char) ee_storage.commands.action);
        if(buttonNum != 127 && ee_storage.commands.action != 127){
        switch(ee_storage.commands.action){
            case NOTE_ON:
                Serial.println("sending note on");
                usbMIDI.sendNoteOn ( ee_storage.commands.data1, ee_storage.commands.data2, ee_storage.commands.data3);
                checkLed(true);
                break;
            case NOTE_OFF:
                Serial.println("sending note off");
                usbMIDI.sendNoteOff ( ee_storage.commands.data1, 0, ee_storage.commands.data3);
                checkLed(false);
                break;
            case NOTE_TOGGLE:
                if(!noteToggle){
                    Serial.println("sending note toggle on");
                    usbMIDI.sendNoteOn ( ee_storage.commands.data1, ee_storage.commands.data2, ee_storage.commands.data3);
                    noteToggle = true;
                } else{
                    Serial.println("sending note toggle off");
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
            break;
            
    }
}


void DLCommand::checkLed(bool onOff){
    if(buttonNum != 127){
        if(onOff){
            Serial.print("action: ");
            Serial.println((unsigned char)ee_storage.commands.action);
            Serial.print("button action: ");
            Serial.println((unsigned char)ee_storage.commands.button_action);
            Serial.print("mode: ");
            Serial.println((unsigned char)ee_storage.commands.mode);
            Serial.print("setting pixel: ");
            Serial.println(buttonNum);
            led->setColor(WHT);
        } else{
            Serial.print("setting pixel off: ");
            Serial.println(buttonNum);
            led->setColor(NONE);
        }
    }
}