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

	private:
        //PIN CONFIG
        int led_pins[NUM_LOOPERS][LED_PINS];
        
        int control_pins[NUM_LOOPERS][NUM_CONTROLS];
        int instance;
        int bank;
        int current_mode;
        int channel;
        long current_time;
        Looper loopers[NUM_LOOPERS];
        static IntervalTimer blinkTimer;
        static boolean blinking;

};

#endif
