#ifndef DLCOMMAND
#define DLCOMMAND

/**
 * @file Command.h
 * @author Vince Cimo
 * @version 1.0
 */
#include <Arduino.h>
#include "dlconst.h"
class DLCommand {

	public:
        DLCommand(unsigned char _execute_on, unsigned char _command_mode, unsigned char _action, unsigned char _data1, unsigned char _data2, unsigned char looperNum, unsigned char * _mode, unsigned char * _bank, unsigned char * _instance);
        DLCommand();
        unsigned char * mode;
        unsigned char * bank;
        unsigned char * instance;
        unsigned char execute_on;
        unsigned char command_mode;
        unsigned char action;
        unsigned char data1;
        unsigned char data2;
        unsigned char looperNum;
        void sendSysEx(); 
        void execute() const;
        void checkForSpecialCommands() const;
};

#endif
