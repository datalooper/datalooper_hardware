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
#include "DLled.h"
#include "MIDI.h"

class DLCommand {

	public:
                DLCommand();
                DLCommand(uint64_t command, DLled * _led, unsigned char _buttonNum, DLObserver * _dataLooper);
                DLObserver * dataLooper;
                DLled* led;
                unsigned char * state;
                bool toggle = false;
                void sendSysEx(); 
                void execute();
                void checkLed(bool onOff);
                void checkDLCommands();
                ee_storage_typ ee_storage;
                // bool waitingForWrite = true;
};

#endif
