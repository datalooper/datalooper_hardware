#ifndef DATALOOPER
#define DATALOOPER

/**
 * @file DataLooper.h
 * @author Vince Cimo
 * @version 1.0
 */
#include <Arduino.h>
#include <EEPROM.h>
#include "IntervalTimer.h"
#include "State.h"
#include <Button.h>
#include "dlconst.h"
#include <WS2812Serial.h>
#include "Observer.h"
class DataLooper :  public DLObserver {

	public:

	DataLooper(WS2812Serial * _leds);
        void init();
        void onControlChange(uint8_t channel, uint8_t control, uint8_t value);
        WS2812Serial * leds;
        unsigned char globalCommands[NUM_GLOBAL_CONFIG_BYTES];
        void loadConfig();
        void onSysEx(const uint8_t *sysExData, uint16_t sysExSize, bool complete);
        void onProgramChange(byte channel, byte program);
        void configureDL(const uint8_t * sysExData);
        void writeCommand(uint8_t actionNum, ee_storage_typ command);
        void offBeat();
        void scanForButtonActivity(unsigned long current_time);
        void clearControlChanges(unsigned char _ccNum, unsigned char _ccValue);
        void changeMode();
        void checkForModeChange();
        Button buttons[NUM_BUTTONS];
        unsigned char ppqCounter = 0;
        void loadAltModeCommands();
        void onPresetChange();
        void beginMIDI();
        void sendNoteOn(uint8_t inNoteNumber,
                           uint8_t inVelocity,
                           uint8_t inChannel);
        void sendNoteOff(uint8_t inNoteNumber,
                            uint8_t inVelocity,
                            uint8_t inChannel);
        void sendProgramChange(uint8_t inProgramNumber,
                                  uint8_t inChannel);
        void sendControlChange(uint8_t inControlNumber,
                                  uint8_t inControlValue,
                                  uint8_t inChannel);
        void sendSysEx(unsigned inLength,
                          const byte* inArray,
                          bool inArrayContainsBoundaries = false);
	private:
        //PIN CONFIG
        unsigned char channel;
        // long current_time;
        // static IntervalTimer blinkTimer;
        // static boolean blinking;
        elapsedMillis blinkTimer = 0;
        void mapColors(unsigned char buttonNum, unsigned char red, unsigned char green, unsigned char blue);
        void requestState();
        byte readByte(unsigned int curByte);
        void writeCommand(uint8_t buttonNumber, uint8_t commandNum, DLCommand command);
        void checkForWriteCompletion();
        void endConfig();
        void writeGlobalConfig();
        void writeCommands(bool shouldWrite);
};

#endif
