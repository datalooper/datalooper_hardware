#include "DLCommand.h"

DLCommand::DLCommand(unsigned char execute_on, unsigned char command_mode, unsigned char action, unsigned char data1, unsigned char data2) : execute_on(execute_on), command_mode(command_mode), action(action), data1(data1), data2(data2)
{
}

void DLCommand::execute() const{
    checkForSpecialCommands();
    Serial.print("executing command");
    Serial.println();
    Serial.print(execute_on);
    unsigned char iterative_value = (instance * (NUM_BANKS * NUM_LOOPERS * NUM_CONTROLS))+(bank * NUM_LOOPERS * NUM_CONTROLS) + data1;
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
            Serial.print("attempting sysex");
            uint8_t data[BYTES_PER_COMMAND] = {action, data1, data2};
            usbMIDI.sendSysEx (BYTES_PER_SYSEX_MSG, data, false);
            break;
    }
    
}

void DLCommand::checkForSpecialCommands() const{
    switch (action){
        case CHANGE_MODE:
            mode = data1;
            break;
        case CHANGE_BANK:
            bank = data1;
            break;
        case CHANGE_INSTANCE:
            instance = data1;
            break;
        default:
            break;
    }
}