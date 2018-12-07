#include "Command.h"

Command::Command()
{}
// void Command::sendSysEx(int looper, int control, byte action, byte long_press_seconds){
//   byte sending = 0x12;
//   byte array[] = {0x41, (byte) instance, (byte) looper, (byte) control, sending, action, long_press_seconds, 0xF7};
//   usbMIDI.sendSysEx(7, array, false);
// }

void Command::execute(){
    
}