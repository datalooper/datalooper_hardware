#include "State.h"

unsigned char State::mode = 0;
unsigned char State::globalChannel = 1;
unsigned char State::dawConnected = 0;
unsigned char State::lastMode = 0;
unsigned char State::inConfig = 0;
unsigned char State::preset = 0;
unsigned char State::presetChanging = false;
char State::currentConfigPreset = 0;
elapsedMillis State::configExitMillis = 0;
unsigned char State::presetsWritten = 0;
bool State::transportRunning = false;
bool State::blinking = false;
bool State::modeChanging = false;

