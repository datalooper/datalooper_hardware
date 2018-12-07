#ifndef COMMAND
#define COMMAND

/**
 * @file Command.h
 * @author Vince Cimo
 * @version 1.0
 */

#include <Arduino.h>

class Command {

	public:
		Command();
        int memLocation;
        int dataType;
        int data1;
        int data2;
        void sendSysEx(); 
        void execute();
};

#endif
