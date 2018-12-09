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
		DLCommand(unsigned char execute_on, unsigned char command_mode, unsigned char action, unsigned char data1, unsigned char data2);
        unsigned char execute_on;
        unsigned char action;
        unsigned char command_mode;
        unsigned char data1;
        unsigned char data2;
        void sendSysEx(); 
        void execute() const;
        void checkForSpecialCommands() const;
};

#endif
