#include "LED.h"

Led::Led(){

}

void Led::init(unsigned char* ledPins){
  
  for(int n = 0; n < 3; n++){
    pinMode(*(ledPins + n), OUTPUT);
    pins[n] = *(ledPins + n);
  }
  curColor = WHT;
}
void Led::writeColor(unsigned char color){
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
void Led::setColor(unsigned char color) {
  curColor = color;
  writeColor(color);
}

void Led::writeColor(unsigned char red, unsigned char green, unsigned char blue){
  analogWrite(pins[0], red);
  analogWrite(pins[1], green);
  analogWrite(pins[2], blue);
}
void Led::restoreColor() {
  writeColor(curColor);
}


