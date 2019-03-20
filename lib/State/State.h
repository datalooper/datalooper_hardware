#ifndef STATE_h
#define STATE_h
#include <elapsedMillis.h>
class State
{
  public:
    static unsigned char mode;
    static unsigned char bank;
    static unsigned char onBeat;
    static unsigned char instance;
    static unsigned char dawConnected;
    static unsigned char lastMode;
    static unsigned char lastBank;
    static unsigned char lastInstance;
    static unsigned char inConfig;
    static unsigned char presetsWritten;
    static bool modeChanging;
    static elapsedMillis configExitMillis;
    static char currentConfigPreset;
    static bool transportRunning;
    static bool blinking;
    static unsigned char preset;
    static unsigned char presetChanging;
    static unsigned char globalChannel;
};

#endif
