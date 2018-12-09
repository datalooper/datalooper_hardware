#ifndef DATALOOPER
#define DATALOOPER

/**
 * @file DataLooper.h
 * @author Vince Cimo
 * @version 1.0
 */

#include "Looper.h"

class DataLooper {

	public:

		DataLooper();
        void init();
        void loadConfig();
		void checkConfig(long current_time);
        void enterConfig();
        void configureLoopers(int i,int n);
        void diagnoseButton(int i, int n, int num);
        void toggleNewSession(boolean on);
        void onSysEx(const uint8_t *sysExData, uint16_t sysExSize, bool complete);
        void onProgramChange(byte channel, byte program);
        void sendSysEx(int looper, int control, byte action, byte long_press_seconds);
        void scanForButtonActivity(long current_time);
        void blink();
        static void endBlink();
        void altModeCommands();

	private:
        //PIN CONFIG
        unsigned char led_pins[NUM_LOOPERS][LED_PINS] = {{3, 4, 6}, {9, 10, 16}, {17, 20, 22}};
        unsigned char control_pins[NUM_LOOPERS][NUM_CONTROLS] = {{0, 1, 2, 5}, {7, 8, 11, 12}, {13, 14, 15, 18}};
        unsigned char channel;
        long current_time;
        Looper loopers[NUM_LOOPERS];
        static IntervalTimer blinkTimer;
        static boolean blinking;

};

#endif
