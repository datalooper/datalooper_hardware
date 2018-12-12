#include "DLCommand.h"

DLCommand::DLCommand() : execute_on(255), command_mode(255), action(255), data1(255), data2(255) {} 
DLCommand::DLCommand(unsigned char _execute_on, unsigned char _command_mode, unsigned char _action, unsigned char _data1, unsigned char _data2, unsigned char _looperNum, unsigned char * _mode, unsigned char * _bank, unsigned char * _instance, unsigned char *_state) : execute_on(_execute_on), command_mode(_command_mode), action(_action), data1(_data1), data2(_data2), looperNum(_looperNum), mode(_mode), bank(_bank), instance(_instance), state(_state)
{
 
}


void DLCommand::execute() const{
    Serial.println();
    Serial.print("MODE: ");
    Serial.print(*mode);
    if(command_mode == *mode){
        checkForSpecialCommands();
        Serial.println();
        Serial.print("execute on: ");
        Serial.print(execute_on);
        Serial.print(" , action: ");
        Serial.print(action);
        Serial.print(" , data1: ");
        Serial.print(data1);
        Serial.print(" , data2: ");
        Serial.print(data2);
        Serial.println();
        unsigned char iterative_value = (*instance * (NUM_BANKS * NUM_LOOPERS * NUM_CONTROLS))+(*bank * NUM_LOOPERS * NUM_CONTROLS) + data1;
        switch(execute_on % NUMBER_DATATYPES){
            case NOTE_ON:
                usbMIDI.sendNoteOn ( iterative_value, data2, MIDI_CHAN);
                break;
            case NOTE_OFF:
                usbMIDI.sendNoteOff ( iterative_value, 0, MIDI_CHAN);
                break;
            case CC:
                usbMIDI.sendControlChange ( iterative_value, data2, MIDI_CHAN);
                break;
            case PROGRAM_CHANGE:
                usbMIDI.sendProgramChange ( iterative_value, MIDI_CHAN);
                break;
            case SYSEX:
                byte sending = 0x12;
                Serial.print((byte) action);
                byte array[] = {0x41, (byte) *instance, (byte) *bank, (byte) looperNum, (byte) *mode, (byte) action, (byte) data1, (byte) data2, sending, 0xF7};
                usbMIDI.sendSysEx(9, array, false);
                break;
        }
    }
    
}

void DLCommand::checkForSpecialCommands() const{
    switch (action){
        case CHANGE_MODE:
            Serial.print("changing mode");
            *mode = data1;
            break;
        case CHANGE_BANK:
            if ((data1 == 1 && *state == STATE_CLEAR) || data1 == 0){
                *bank = looperNum;
            }
            
            break;
        case CHANGE_INSTANCE:
            Serial.print("Changing instance");
            *instance = data1;
            break;
        default:
            break;
    }
}