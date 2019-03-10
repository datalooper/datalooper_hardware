#include <Arduino.h>
#include <dlconst.h> 
#include <DataLooper.h>
#include <Wire.h>
#include <MIDI.h>


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
void onCC(uint8_t channel, uint8_t control, uint8_t value){
  dataLooper.onControlChange(channel, control, value);
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
  Wire.begin(); // initialise the connection
  Wire.setClock(400000);
  Serial.begin(9600);
  usbMIDI.begin();
  // registers sysex handler
  usbMIDI.setHandleSystemExclusive(onSysEx);
  usbMIDI.setHandleControlChange(onCC);

  // registers program change handler
  usbMIDI.setHandleProgramChange(onProgramChange);
  usbMIDI.setHandleStart(onStart);
  usbMIDI.setHandleStop(onStop);
  dataLooper.loadCommands();
  dataLooper.beginMIDI();

}

void loop()
{
  unsigned long current_time = millis();
  dataLooper.scanForButtonActivity(current_time); 
  // Serial.print("alive");
    while (usbMIDI.read()) {
  }
}
