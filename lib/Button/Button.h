#ifndef BUTTON
#define BUTTON

/**
 * @file Button.h
 * @author Vince Cimo
 * @version 1.0
 */

#include <Arduino.h>
#include "DLCommand.h"
#define BOUNCE_WITH_PROMPT_DETECTION
#include "Bounce2.h"
#include "dlconst.h"
#include <array>
#include <State.h>
#include <vector>
#include <WS2812Serial.h>
#include <Observer.h>     

class Button {
	public:
        Button();
        void init(unsigned char control_pin, unsigned char _buttonNumber, unsigned char _ledNumber, WS2812Serial* _leds, DLObserver* _dataLooper);
         void onPress(unsigned long current_time);
         void onRelease();
         void onMultiPress();
         void onMultiRelease();
         void onLongPress();
         void onLongRelease();
         void update(unsigned long current_time);
         void loadCommands();
         bool checkCommands(unsigned char execute_on);
         void onLooperStateChange(unsigned char _looperNum, unsigned char _looperState);
         void clearControlChanges(unsigned char _ccNum, unsigned char _ccValue);
         void onBeat(unsigned char beatNum);
         bool isLooperRecordCommand();
         WS2812Serial *leds;
         DLCommand commands[NUMBER_COMMANDS];
         unsigned char buttonNumber;
         unsigned char ledNumber;
         unsigned int press_time;
         unsigned char currentConfigCommand;
         boolean is_pressed;
         bool long_pressed;
         unsigned short int long_press_time;
         Bounce bounce;
         DLObserver* dataLooper;
         colorType lastColor;
         colorType curColor;
         void setColor(colorType color);
         void writeColor(colorType color);
         void restoreColor();
         void unBlink();
         elapsedMillis blinkTimer = 0;
         void changeMode();
         bool checkNewSessionCommands();
         void diagnoseCommand(unsigned char execute_on, unsigned char x);

};

#endif
