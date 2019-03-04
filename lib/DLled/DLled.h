#ifndef DLLED
#define DLLED

#include <WS2812Serial.h>
#include <dlconst.h>
class DLled{
    public:
        unsigned char buttonNum;
        WS2812Serial* leds;
        DLled();
        void init(WS2812Serial * _leds, unsigned char _buttonNum);
        colorType lastColor;
        colorType curColor;
        void setColor(colorType color);
        void writeColor(colorType color);
        void restoreColor();
};
#endif