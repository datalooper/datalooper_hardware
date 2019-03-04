#include <WS2812Serial.h>
#include <Arduino.h>

const int numled = 12;
const int pin = 24;
DMAMEM byte displayMemory[numled*12]; // 12 bytes per LED
byte drawingMemory[numled*3];         //  3 bytes per LED
WS2812Serial leds = WS2812Serial(numled, displayMemory, drawingMemory, pin, WS2812_GRB);

void setup()
{
  leds.begin();
  Serial.begin(9600);
}

void loop()
{
  Serial.print("looping");
  while (usbMIDI.read()) {
  }
}
