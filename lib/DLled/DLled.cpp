#include <DLled.h>
DLled::DLled(){}
void DLled::init(WS2812Serial * _leds, unsigned char _buttonNum) {
    leds = _leds;
    buttonNum = _buttonNum;
}

 void DLled::   setColor(colorType color){
     curColor = color;
    //  if(color.rgb != NONE.rgb){
    //     Serial.print("red: ");
    //     Serial.println(curColor.colors.red);
    //     Serial.print("blue: ");
    //     Serial.println(curColor.colors.blue);
    //     Serial.print("green: ");
    //     Serial.println(curColor.colors.green);
    // }
    writeColor(curColor);
 }

 void DLled::restoreColor(){
     writeColor(curColor);
 }
void DLled::writeColor(colorType color){
     leds->setPixel(LED_NUMBERS[buttonNum], color.rgb);
     leds->show();
}