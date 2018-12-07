#include "LED.h"

Led::Led(){

}

void Led::init(int* ledPins){
  
  for(int n = 0; n < 3; n++){
    pinMode(*(ledPins + n), OUTPUT);
    this->ledPins[n] = *(ledPins + n);
  }
  setColor(PURPLE);
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
  // Serial.print("Pin 1: ");
  // Serial.print(ledPins[0]);
  // Serial.println();
  // Serial.print("Pin 2: ");
  // Serial.print(ledPins[1]);
  // Serial.println();
  // Serial.print("Pin 3: ");
  // Serial.print(ledPins[2]);
  // Serial.println();
  analogWrite(ledPins[0], red);
  analogWrite(ledPins[1], green);
  analogWrite(ledPins[2], blue);
}
void Led::restoreColor() {
  writeColor(curColor);
}


