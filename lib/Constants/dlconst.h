#ifndef CONSTANTS
#define CONSTANTS


//GLOBAL CONST
const unsigned char NUM_LOOPERS = 3;
const unsigned char NUM_BANKS = 3;
const unsigned char LED_PINS = 3;
const unsigned char NUM_CONTROLS = 4;

//COLORS
const unsigned char BLUE = 0;
const unsigned char RED = 1;
const unsigned char GREEN = 2;
const unsigned char YELLOW = 3;
const unsigned char WHT = 4;
const unsigned char NONE = 5;
const unsigned char PURPLE = 6;
const unsigned char CYAN = 7;
const unsigned char GRY = 8;

//MODES
const unsigned char LOOPER_MODE = 0;
const unsigned char NEW_SESSION_MODE = 1;
const unsigned char SCENE_MODE = 2;
const unsigned char ALT_SCENE_MODE = 3;
const unsigned char CONFIG_MODE = 4;
const unsigned char NUMBER_MODES = 5;

static const long DOUBLE_HIT_TIME = long(500);
static const long LONG_PRESS_TIME = long(500);

// the MIDI channel number to send messages
static const unsigned char MIDI_CHAN = 14;

//DOWNBEAT BLINK TIME
static const unsigned char BLINK_TIME = 50;

//STATE CONST
// static const unsigned char STOPPED = 0;
// static const unsigned char RECORDING = 1;
// static const unsigned char PLAYING = 2;
// static const unsigned char OVERDUBBING = 3;
// static const unsigned char CLEAR = 4;

//LOOPER COMMAND
static const unsigned char RESET = 0;
static const unsigned char CHANGE_STATE = 1;
static const unsigned char DOWNBEAT = 2;
static const unsigned char REQUEST_NOTES = 3;
static const unsigned char NEW_SESSION = 4;
static const unsigned char CONFIG = 5;


static const unsigned char NUMBER_COMMANDS = 3;
static const unsigned char BYTES_PER_COMMAND = 4;
static const unsigned char BYTES_PER_SYSEX_MSG = 3;




//MIDI TYPES
const unsigned char NOTE_ON = 0;
const unsigned char NOTE_OFF = 1;
const unsigned char CC = 2;
const unsigned char PROGRAM_CHANGE = 3;
const unsigned char SYSEX = 4;
const unsigned char NUMBER_DATATYPES = 5;


//button debounce time
const unsigned char DEBOUNCE_TIME = 25;

//DataTypes
const unsigned char PRESS_NOTE_ON = 0;
const unsigned char PRESS_NOTE_OFF = 1;
const unsigned char PRESS_CC = 2;
const unsigned char PRESS_PROGRAM_CHANGE = 3;
const unsigned char PRESS_SYSEX = 4;
const unsigned char RELEASE_NOTE_ON = 5;
const unsigned char RELEASE_NOTE_OFF = 6;
const unsigned char RELEASE_CC = 7;
const unsigned char RELEASE_PROGRAM_CHANGE = 8;
const unsigned char RELEASE_SYSEX = 9;
const unsigned char DOUBLE_TAP_NOTE_ON = 10;
const unsigned char DOUBLE_TAP_NOTE_OFF = 11;
const unsigned char DOUBLE_TAP_CC = 12;
const unsigned char DOUBLE_TAP_PROGRAM_CHANGE = 13;
const unsigned char DOUBLE_TAP_SYSEX = 14;
const unsigned char PRESS_RELEASE_NOTE_ON_OFF = 15;
const unsigned char PRESS_RELEASE_CC_VAL_0 = 16;
const unsigned char LONG_PRESS_NOTE_ON = 17;
const unsigned char LONG_PRESS_NOTE_OFF = 18;
const unsigned char LONG_PRESS_CC = 19;
const unsigned char LONG_PRESS_PROGRAM_CHANGE = 20;
const unsigned char LONG_PRESS_SYSEX = 21;

//ACTIONS

const unsigned char RECORD = 0;
const unsigned char STOP = 1;
const unsigned char UNDO = 2;
const unsigned char CLEAR = 3;
const unsigned char MUTE_TRACK = 4;
const unsigned char NEW_CLIP = 5;
const unsigned char CHANGE_BANK = 6;
const unsigned char CHANGE_INSTANCE = 7;
const unsigned char CLEAR_ALL = 17;
const unsigned char MUTE_ALL = 19;
const unsigned char TOGGLE_STOP_START = 20;
const unsigned char TAP_TEMPO = 25;
const unsigned char CHANGE_MODE = 27;

const unsigned char NUM_TAPS_BEFORE_METRO = 4;

const unsigned char ONLY_BANK_WHEN_CLEAR = 1;
const unsigned char ALWAYS_BANK = 0;

const unsigned char DL_TRACK_TYPE = 0;
const unsigned char CL_TRACK_TYPE = 1;
const unsigned char BOTH_TRACK_TYPES = 2;

const unsigned char UNQUANTIZED = 0;
const unsigned char QUANTIZED = 1;

const unsigned char OFF = 0;
const unsigned char ON = 1;
const unsigned char TOGGLE = 2;






#endif