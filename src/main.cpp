#include "DataLooper.h"

void setup()
{
  Serial.begin(9600); 
  DataLooper::loadConfig();
  DataLooper::init();
  // registers sysex handler
  usbMIDI.setHandleSystemExclusive(DataLooper::onSysEx);
  // registers program change handler
  usbMIDI.setHandleProgramChange(DataLooper::onProgramChange);
}

void loop()
{
  usbMIDI.read();
  unsigned long current_time = millis();
  DataLooper::checkIfOnBeat(current_time);
  DataLooper::blinkLEDs(current_time);
  DataLooper::checkConfig(current_time);
  DataLooper::scanForButtonActivity(current_time);  
}









