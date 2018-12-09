#include "DataLooper.h"
#include <Arduino.h>

DataLooper dataLooper;

void onSysEx(const uint8_t *sysExData, uint16_t sysExSize, bool complete)
{
  Serial.print("in Sysex");
  Serial.println();
  dataLooper.onSysEx(sysExData, sysExSize, complete);
}
void onProgramChange(byte channel, byte program){
  dataLooper.onProgramChange(channel, program);
}

void setup()
{
  Serial.begin(9600); 
  
  // registers sysex handler
  usbMIDI.setHandleSystemExclusive(onSysEx);
  dataLooper.init();
  // registers program change handler
  usbMIDI.setHandleProgramChange(onProgramChange);
}

void loop()
{
  usbMIDI.read();
  unsigned long current_time = millis();
  dataLooper.scanForButtonActivity(current_time); 
}
