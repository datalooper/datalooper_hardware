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
#include <vector>
class Button {

	public:
		Button();
        void init(unsigned char* control_pin, unsigned char buttonNum);
        void onPress(unsigned long current_time);
        void onRelease();
        void onMultiPress();
        void onMultiRelease();
        void onLongPress();
        void update(bool isFlipped, unsigned long current_time);
        void addCommand(DLCommand *command);
        void loadCommands();
        void checkCommands(unsigned char low, unsigned char high);
        std::vector<DLCommand*> commands;
        unsigned char buttonNumber;
        unsigned int press_time;
        boolean is_pressed;
        unsigned short int long_press_time;

        Bounce bounce;
        
};

#endif
