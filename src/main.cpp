#include <Arduino.h>
#include <dlconst.h> 
#include <DataLooper.h>
#define E2END 0xFE
DMAMEM byte displayMemory[NUM_BUTTONS*12]; // 12 bytes per LED
byte drawingMemory[NUM_BUTTONS*3];         //  3 bytes per LED
WS2812Serial leds = WS2812Serial(NUM_BUTTONS, displayMemory, drawingMemory, LED_PIN, WS2812_GRB);

DataLooper dataLooper(&leds);
unsigned char ppqcounter = 0;
unsigned char pixel = 0;
bool downbeat = false;
unsigned char curBeat = 0;
void onSysEx(const uint8_t *sysExData, uint16_t sysExSize, bool complete)
{
  dataLooper.onSysEx(sysExData, sysExSize, complete);
}
void onProgramChange(byte channel, byte program){
  dataLooper.onProgramChange(channel, program);
}

void onStart(){
  State::transportRunning = true;
}

void onStop(){
  State::transportRunning = false;
}

void setup()
{
  leds.begin();
  Serial.begin(9600);
  usbMIDI.begin();
  // registers sysex handler
  usbMIDI.setHandleSystemExclusive(onSysEx);

  // registers program change handler
  usbMIDI.setHandleProgramChange(onProgramChange);
  usbMIDI.setHandleStart(onStart);
  usbMIDI.setHandleStop(onStop);

}

void loop()
{
  unsigned long current_time = millis();
  dataLooper.scanForButtonActivity(current_time); 
    while (usbMIDI.read()) {
  }
}
