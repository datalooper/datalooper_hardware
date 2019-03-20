#ifndef CONSTANTS
#define CONSTANTS

//GLOBAL CONST
const unsigned char NUM_BUTTONS = 12;
const unsigned char LED_PIN = 24;

// const unsigned char BUTTON_PINS[NUM_BUTTONS] = {8,9,1,11,4,5,6,7,0,15,2,12};

//Pre MIDI Fix Pins
const unsigned char BUTTON_PINS[NUM_BUTTONS] = {8,9,10,11,4,5,6,7,0,1,2,12};
const unsigned char LED_NUMBERS[NUM_BUTTONS] = {8,9,10,11,7,6,5,4,0,1,2,3};

static const unsigned int DOUBLE_HIT_TIME = 300;
static const unsigned int LONG_PRESS_TIME = 500;


//DOWNBEAT BLINK TIME
static const unsigned char BLINK_TIME = 100;

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
const unsigned char NUM_GLOBAL_CONFIG_BYTES = 8;

constexpr struct LState
{
	int STOPPED;
	int RECORDING;
	int PLAYING;
	int OVERDUBBING;
	int CLEAR;
	int OFF;
} looper_state = { 0, 1, 2, 3, 4, 5};

constexpr struct modes
{
	int USER_MODE;
	int NEW_SESSION_MODE;
	int CLIP_LAUNCH_MODE;
	int ALL_BUT_USER;
} MODES = {0, 1, 2, 3};

constexpr struct buttonActions
{
	int PRESS;
	int RELEASE;
	int MULTI_PRESS;
	int MULTI_RELEASE;
	int LONG_PRESS;
	int LONG_RELEASE;
} BUTTON_ACTIONS = {0, 1, 2, 3, 4, 5};

constexpr struct actions
{
	int METRO_CONTROL;
	int LOOPER_CONTROL;
	int TAP_TEMPO;
	int FIXED_CLIP_CONTROL;	
	int STOP_START_ALL;
	int MUTE_CONTROL;
	int TRANSPORT_CONTROL;
	int SCENE_CONTROL;
	int CHANGE_MODE;
	int CHANGE_INSTANCE;
	int MOVE_SESSION_HIGHLIGHT;
	int REQUEST_STATE;
	int REQUEST_MIDI_MAP_REBUILD;
} ACTIONS = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};

constexpr struct sysex_commands
{
	int DAW_CONNECTION;
	int STATE_CHANGE;
	int REQUEST_MIDI;
	int ON_BEAT;	
	int CHANGE_COLOR;
	int CHANGE_BLINK;
	int START_CONFIG;
	int CONFIG_ACTION_BYTE;
	int CONFIG_MIDI_SENT;
	int CONFIG_GLOBAL_BYTE;
	int GLOBAL_SETTINGS_SENT;
} SYSEX_COMMANDS = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

constexpr struct global_settings
{
	int GLOBAL_CHANNEL;
	int STARTING_MODE;
} GLOBAL_SETTINGS = {0, 1};

const unsigned char DATALOOPER_IDENTIFIER = 0x1E;


constexpr struct dlactions
{
	int CHANGE_MODE;
	int CHANGE_PRESET;
} DLACTIONS = {0, 1};


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

#endif
