#include "LED.h"

Led::Led(int ledPins[])
{
  pins = ledPins;
  for(int n = 0; n < LED_PINS; n++){
    pinMode(pins[n], OUTPUT);
  }
}
void Led::writeColor(int color){
      if (color == GREEN) {
        writeColor(255,100,255);
      } else if (color == RED) {
        writeColor(100,255,255);
      } else if (color == BLUE) {
        writeColor(255,255,100);
      } else if (color == YELLOW) {
        writeColor(100,100,255);
      } else if (color == WHT) {
        writeColor(100,100,100);
      } else if (color == NONE) {
        writeColor(255,255,255);
      } else if (color == PURPLE) {
        writeColor(100,255,100);
      } else if (color == CYAN) {
        writeColor(255,100,100);
      } else if (color == GRY) {
        writeColor(200,200,0);
      }
    }
void Led::setColor(int color) {
  curColor = color;
  writeColor(color);
}
void Led::writeColor(int red, int green, int blue){
  analogWrite(pins[0], red);
  analogWrite(pins[1], green);
  analogWrite(pins[2], blue);
}
void Led::restoreColor() {
  setColor(curColor);
}
