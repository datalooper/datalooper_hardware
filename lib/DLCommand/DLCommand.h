#ifndef DLCOMMAND
#define DLCOMMAND

/**
 * @file Command.h
 * @author Vince Cimo
 * @version 1.0
 */
#include <Arduino.h>
#include "dlconst.h"
#include <WS2812Serial.h>
#include "Observer.h"
class DLCommand {

	public:
                DLCommand();
                DLCommand(uint64_t command, WS2812Serial * _leds, unsigned char _buttonNum, DLObserver * _dataLooper);
                DLObserver * dataLooper;
                unsigned char buttonNum;
                WS2812Serial* leds;
                unsigned char * state;
                bool noteToggle = false;
                bool ccToggle = false;
                void sendSysEx(); 
                void execute();
                void checkLed(bool onOff);
                void checkDLCommands();
                ee_storage_typ ee_storage;
};

#endif
