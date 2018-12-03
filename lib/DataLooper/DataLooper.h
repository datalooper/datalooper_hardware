#ifndef DATALOOPER
#define DATALOOPER

/**
 * @file DataLooper.h
 * @author Vince Cimo
 * @version 1.0
 */

#include <Arduino.h>
#include "Looper.h"

class DataLooper {

	public:

		DataLooper();
        static void init();
        static void loadConfig();
        static void checkIfOnBeat(long current_time);
		static void blinkLEDs(long current_time);
		static void checkConfig(long current_time);
        static void onButtonPress(int looper, int control, long current_time);
        static void onButtonRelease(int looper, int control, long current_time);
        static void onMultiPress(int looper, int control, long current_time);
        static void onMultiRelease(int looper, int control, long current_time);
        static void onLongPress(int looper, int control, int long_press_time);
        static void enterConfig();
        static void configureLoopers(int i,int n);
        static void diagnoseButton(int i, int n, int num);
        static void toggleNewSession(boolean on);
        static void onSysEx(const uint8_t *sysExData, uint16_t sysExSize, bool complete);
        static void onProgramChange(byte channel, byte program);
        static void sendSysEx(int looper, int control, byte action, byte long_press_seconds);
        static void scanForButtonActivity(long current_time);

	private:
        //SYSEX TYPES
        static const int BUTTON_PRESS = 0;
        static const int BUTTON_RELEASE = 1;
        static const int LONG_PRESS = 2;
        static const int INSTANCE_CHANGE = 3;
        static const int MULTI_TAP = 4;
        static const int MULTI_RELEASE = 5;

        static const long DOUBLE_HIT_TIME = long(500);

        // the MIDI channel number to send messages
        static const int MIDI_CHAN = 14;

        //DOWNBEAT BLINK TIME
        static const int BLINK_TIME = 50;

        //STATE CONST
        static const int STOPPED = 0;
        static const int RECORDING = 1;
        static const int PLAYING = 2;
        static const int OVERDUBBING = 3;
        static const int CLEAR = 4;

        //LOOPER COMMAND
        static const int RESET = 0;
        static const int CHANGE_STATE = 1;
        static const int DOWNBEAT = 2;
        static const int REQUEST_NOTES = 3;
        static const int NEW_SESSION = 4;
        static const int CONFIG = 5;


        static int looperNum;
        static int looperCommand;
        static int instance;
        static int memoryInstance;

        //STATE VARIABLES
        static boolean configActive;
        static boolean newSessionActive;
        static unsigned long configOffDelay;
        static boolean onBeat;
        static unsigned long beat_time;

        //PIN CONFIG
        static int led_pins[NUM_LOOPERS][LED_PINS];
        static int control_pins[NUM_LOOPERS][NUM_CONTROLS];

        static long master_stop_time;
        static long loop_stop_time;
        static long config_press_time;
        static long current_time;
        static Looper *loopers[NUM_LOOPERS];

};

#endif
