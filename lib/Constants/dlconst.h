#ifndef CONSTANTS
#define CONSTANTS


//GLOBAL CONST
const unsigned char NUM_BANKS = 3;
const unsigned char NUM_BUTTONS = 12;
const unsigned char LED_PIN = 24;
const unsigned char BUTTON_PINS[NUM_BUTTONS] = {8,9,1,11,4,5,6,7,0,15,2,12};
const unsigned char LED_NUMBERS[NUM_BUTTONS] = {8,9,10,11,7,6,5,4,0,1,2,3};


//STATE
const unsigned char STATE_CLEAR = 4;
//MODES
const unsigned char LOOPER_MODE = 0;
const unsigned char NEW_SESSION_MODE = 1;
const unsigned char SCENE_MODE = 2;
const unsigned char ALT_SCENE_MODE = 3;
const unsigned char CONFIG_MODE = 4;
const unsigned char NUMBER_MODES = 5;

static const long DOUBLE_HIT_TIME = long(300);
static const long LONG_PRESS_TIME = long(500);

// the MIDI channel number to send messages
static const unsigned char MIDI_CHAN = 14;

//DOWNBEAT BLINK TIME
static const unsigned char BLINK_TIME = 100;

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
static const unsigned char MODE_CHANGE = 4;
static const unsigned char BANK_CHANGE = 5;
static const unsigned char ABLETON_CONNECTED = 6;
static const unsigned char CONFIG = 7;


static const unsigned char NUMBER_USER_COMMANDS = 2;
static const unsigned char NUMBER_COMMANDS = 5;
static const unsigned char NUMBER_PRESETS = 3;
static const unsigned char BYTES_PER_COMMAND = 6;
static const unsigned char SYSEX_BYTES_PER_COMMAND = 9;



//MIDI TYPES
const unsigned char NOTE_ON = 0;
const unsigned char NOTE_OFF = 1;
const unsigned char NOTE_TOGGLE = 2;
const unsigned char CC_TOGGLE = 3;
const unsigned char CC = 4;
const unsigned char PROGRAM_CHANGE = 5;
const unsigned char DATALOOPER_SPECIFIC = 6;
const unsigned char SYSEX = 7;


//button debounce time
const unsigned char DEBOUNCE_TIME = 25;

//DataTypes

//ACTIONS
const unsigned char TAP_TEMPO_BUTTON_NUM = 7;
const unsigned char EXIT_BUTTON_NUM = 3;


const unsigned char sysExStartByte = 4;

constexpr struct LState
{
	unsigned char STOPPED;
	unsigned char RECORDING;
	unsigned char PLAYING;
	unsigned char OVERDUBBING;
	unsigned char CLEAR;
	unsigned char OFF;
} looper_state = { 0, 1, 2, 3, 4, 5};

constexpr struct modes
{
	unsigned char USER_MODE;
	unsigned char NEW_SESSION_MODE;
	unsigned char CLIP_LAUNCH_MODE;
	unsigned char ALL_BUT_USER;
} MODES = {0, 1, 2, 3};

constexpr struct buttonActions
{
	unsigned char PRESS;
	unsigned char RELEASE;
	unsigned char MULTI_PRESS;
	unsigned char MULTI_RELEASE;
	unsigned char LONG_PRESS;
	unsigned char LONG_RELEASE;
} BUTTON_ACTIONS = {0, 1, 2, 3, 4, 5};

constexpr struct actions
{
	unsigned char METRO_CONTROL;
	unsigned char LOOPER_CONTROL;
	unsigned char TAP_TEMPO;
	unsigned char FIXED_CLIP_CONTROL;	
	unsigned char STOP_START_ALL;
	unsigned char MUTE_CONTROL;
	unsigned char TRANSPORT_CONTROL;
	unsigned char SCENE_CONTROL;
	unsigned char CHANGE_MODE;
	unsigned char CHANGE_INSTANCE;
	unsigned char MOVE_SESSION_HIGHLIGHT;
	unsigned char REQUEST_STATE;
} ACTIONS = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

constexpr struct dlactions
{
	int CHANGE_MODE;
	int CHANGE_PRESET;
} DLACTIONS = {0, 1};
const unsigned char NUM_TAPS_BEFORE_METRO = 3;

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

typedef struct
{
		uint64_t button_action : 4;
		uint64_t action : 4;
		uint64_t data1 : 8;
		uint64_t data2 : 8;
		uint64_t data3 : 8;
		uint64_t data4 : 8;
		uint64_t data5 : 4;
		uint64_t led_control : 1;
		uint64_t mode : 3;
} commands_typ;

typedef union
{
		commands_typ commands;
		uint8_t asBytes[BYTES_PER_COMMAND];
		uint64_t the_big_blob;
} ee_storage_typ;

typedef struct
{
	/* data */	
	uint8_t blue : 8;
	uint8_t green : 8;
	uint8_t red : 8;
} rgbType;

typedef union 
{
	unsigned int rgb;
	rgbType colors;
} colorType;

const colorType BLUE = {0x000064};
const colorType RED =  {0x640000};
const colorType GREEN = {0x006400};
const colorType YELLOW = {0x353700};
const colorType WHT = {0x161715};
const colorType NONE = {0x000000};
const colorType PURPLE = {0x120009};
const colorType CYAN = {0x001255};

// //COLORS
// const unsigned int BLUE = (unsigned int) 0x000064;
// const unsigned int RED = (unsigned int) 0x640000;
// const unsigned int GREEN = (unsigned int) 0x006400;
// const unsigned int YELLOW = (unsigned int) 0x353700;
// const unsigned int WHT = (unsigned int) 0x161715;
// const unsigned int NONE = (unsigned int) 0x000000;
// const unsigned int PURPLE = (unsigned int) 0x120009;
// const unsigned int CYAN = (unsigned int) 0x001255;
// const unsigned int GRY = 8;

#endif
