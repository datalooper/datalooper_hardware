#ifndef CONSTANTS
#define CONSTANTS

//GLOBAL CONST
const int NUM_LOOPERS = 3;
const int LED_PINS = 3;
const int NUM_CONTROLS = 4;
const int BLUE = 0;
const int RED = 1;
const int GREEN = 2;
const int YELLOW = 3;
const int WHT = 4;
const int NONE = 5;
const int PURPLE = 6;
const int CYAN = 7;
const int GRY = 8;

//MODES
const int LOOPER_MODE = 0;
const int NEW_SESSION_MODE = 1;
const int SCENE_MODE = 2;
const int ALT_SCENE_MODE = 3;
const int CONFIG_MODE = 4;


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


static const int NUMBER_COMMANDS = 4;
static const int BYTES_PER_COMMAND = 3;

//button debounce time
const int DEBOUNCE_TIME = 25;

#endif