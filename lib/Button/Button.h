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
#include <DLled.h>
#include <i2c_t3.h>

class Button {
	public:
        Button();
        void requestState();
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
         bool onLooperStateChange(unsigned char _looperNum);
         void updateLooperState(unsigned char _looperState);
         void clearControlChanges(unsigned char _ccNum, unsigned char _ccValue);
         void onBeat(unsigned char beatNum);
         bool isLooperCommand(DLCommand command, bool isRecordCommand);
         bool isLooperRecordButton();
         bool isCurrentPresetCommand(DLCommand command);
         void addCommand(ee_storage_typ command, unsigned char commandNum);
         void onControlChange(uint8_t channel, uint8_t control, uint8_t value);
         DLled led;
         DLCommand commands[NUMBER_COMMANDS];
         unsigned char buttonNumber;
         unsigned char ledNumber;
         int press_time;
         unsigned char currentConfigCommand;
         boolean is_pressed;
         bool long_pressed;
         short int long_press_time;
         Bounce bounce;
         DLObserver* dataLooper;
         void fastBlink(bool shouldFastBlink);
         elapsedMillis fastBlinkTimer = 0;
         bool isFastBlinking = false;
         bool shouldBlink = false;
         void toggleBlink(unsigned char _shouldBlink);
         void unBlink();
         void changeMode();
         bool checkNewSessionCommands();
         void checkClipLaunchCommands();
         bool shouldButtonBlink(DLCommand command);
         void diagnoseCommand(unsigned char x);
         void clearPresetToggles(unsigned char presetNum);
         byte readByte(unsigned int eeaddress );
         void startConfig();
         void configureDL(const uint8_t *sysExData);
         void writeCommand(uint8_t actionNum, DLCommand command);
         bool checkForWriteCompletion();
         void storeCommands();
         bool shouldRequestRebuild();
};

#endif
