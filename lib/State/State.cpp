#include "State.h"

unsigned char State::mode = 0;
unsigned char State::bank = 0;
unsigned char State::instance = 0;
unsigned char State::dawConnected = 0;
unsigned char State::lastMode = 0;
unsigned char State::lastBank = 0;
unsigned char State::lastInstance = 0;
unsigned char State::inConfig = 0;
char State::currentConfigButton = -1;
elapsedMillis State::configExitMillis = 0;
bool State::transportRunning = false;
bool State::blinking = false;

