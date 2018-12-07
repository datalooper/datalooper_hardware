#ifndef BUTTON
#define BUTTON

/**
 * @file Button.h
 * @author Vince Cimo
 * @version 1.0
 */

#include <Arduino.h>
#include "Command.h"
#define BOUNCE_WITH_PROMPT_DETECTION
#include "Bounce2.h"
#include "dlconst.h"

class Button {

	public:
		Button();
        void init(int control_pin, int buttonNum);
        void onPress(long current_time);
        void onRelease();
        void onMultiPress();
        void onMultiRelease();
        void onLongPress();
        void update(boolean isFlipped, long current_time);
        void loadCommands();
        void checkCommands(int low, int high);
        Command commands[NUMBER_COMMANDS];
        int buttonNumber;
        long press_time;
        boolean is_pressed;
        long long_press_time;

        Bounce bounce;
        
};

#endif
