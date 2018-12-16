#include <Arduino.h>
#include "DataLooper.h"
#include <EEPROM.h>
#include "IntervalTimer.h"
/**
 * @file DataLooper.cpp
 * @author Vince Cimo
 * @version 1.0
 */

boolean DataLooper::blinking = false;
IntervalTimer DataLooper::blinkTimer;

DataLooper::DataLooper() : instance(0), bank(0), mode(0), lastBank(bank), lastMode(mode), lastInstance(instance) {
  blinking = false;
  //initializes new loopers
  loadConfig();
  altModeCommands();
}
void DataLooper::init(){
     for (unsigned char i = 0; i < NUM_LOOPERS; i++)
    {
      loopers[i].init(&led_pins[i][0], &control_pins[i][0],i);
    }
}
void DataLooper::loadConfig(){
	// INSTANCE
	instance = EEPROM.read(127);
	if(instance == 255){
		EEPROM.write(127, 0);
	}
  // CHANNEL
  channel = EEPROM.read(126);
  if(channel == 255){
    //channel 14 default
    EEPROM.write(126,14);
  }

  // DEFAULT MODE ON STARTUP
  mode = EEPROM.read(125);
  if(mode == 255){
    //looper mode default
    EEPROM.write(125,0);
  }

  //EEPROM BUTTON COMMANDS LOADED IN INDIVIDUAL CLASS
}

void DataLooper::scanForButtonActivity(long current_time){
    checkForBankChange();
    checkForModeChange();
    
    for (unsigned char i = 0; i < NUM_LOOPERS; i++)
    {
      loopers[i].updateButtons(current_time);  
      if(blinking && i == bank){
          loopers[i].led.writeColor(NONE);
      } else{
        loopers[i].led.restoreColor();
      }
    }
}

void DataLooper::checkForBankChange(){
  if (bank != lastBank){
    for (unsigned char i = 0; i < NUM_LOOPERS; i++)
    {
      loopers[i].updateState(STATE_CLEAR);
    }
    lastBank = bank;
  }
}

void DataLooper::checkForModeChange(){
  if (mode != lastMode){

    switch (mode){
      case NEW_SESSION_MODE:
        setColor(PURPLE);
        break;
      case LOOPER_MODE:
        setColor(WHT);
        break;
      case CONFIG_MODE:
        setColor(CYAN);
        break;
      default:
        break;
    }
    lastMode = mode;
  }
}
void DataLooper::setColor(unsigned char color){
  for (unsigned char i = 0; i < NUM_LOOPERS; i++)
    {
      loopers[i].led.setColor(color);
    }
}
void DataLooper::enterConfig(){
  Serial.print("Entering config");
  for (unsigned char l = 0; l < NUM_LOOPERS; l++){
    loopers[l].led.setColor(CYAN);
  }
  
}
void DataLooper::configureLoopers(int i,int n){
  instance = ( i * NUM_CONTROLS) + n;
  Serial.print("configuring looper as: ");
  Serial.print(instance);
  EEPROM.write(0, instance);
  for( unsigned char x = 0; x < NUM_LOOPERS; x++){
    // loopers[x].configureLooper(led_pins[x], control_pins[x], x);
    loopers[x].led.setColor(GREEN);
    //configOffDelay = millis();
  }
  sendSysEx(127,127,127,127);
}
// void DataLooper::diagnoseButton(int i, int n, int num){
//   Serial.print("looper:");
//   Serial.print( i );
//   Serial.println();
//   Serial.print("button: ");
//   Serial.print(n);
//   Serial.println();
//   Serial.print("cc#: ");
//   Serial.print(num);
//   Serial.println();
// }

void DataLooper::changeMode(unsigned char newMode){
  mode = newMode;
}

void DataLooper::changeBank(unsigned char newBank){
  bank = newBank;
}

void DataLooper::onSysEx(const uint8_t *sysExData, uint16_t sysExSize, bool complete)
{
  //Translate SYSEX to variable data
  unsigned char looperNum = sysExData[1];
  unsigned char looperCommand = sysExData[2];
  unsigned char looperData = sysExData[4];
  unsigned char localLooper = looperNum % NUM_LOOPERS;
  unsigned char reqInstance = (looperNum - localLooper ) / (NUM_LOOPERS * NUM_BANKS);
  unsigned char reqBank = (( looperNum - localLooper ) / NUM_LOOPERS ) % NUM_BANKS;
  switch (looperCommand)
    {
      // case RESET:
      //     toggleNewSession(false);
      //     for( int x = 0; x < NUM_LOOPERS; x++){
      //       loopers[x].led.setColor(WHT);
      //     }
      case DOWNBEAT:
          blink();
          break;
      case CHANGE_STATE:
        if(reqBank == bank && reqInstance == instance){
            Serial.print("State change req");
            Serial.print(looperData);
            Serial.println();
            Serial.print("looper #: ");
            Serial.print(localLooper);
            Serial.println();
            loopers[localLooper].updateState(looperData);
          }
          break;
      case REQUEST_NOTES:
          usbMIDI.sendNoteOn ( ((looperNum ) * NUM_CONTROLS ) + looperData, 127, MIDI_CHAN);
          usbMIDI.sendNoteOff ( ((looperNum ) * NUM_CONTROLS ) + looperData, 127, MIDI_CHAN);
          break;
      case MODE_CHANGE:
          Serial.print("Mode change req");
          Serial.print(looperData);
          changeMode(looperData);
          break;
      case BANK_CHANGE:
          Serial.print("bank change req");
          Serial.print(looperData);
          changeBank(looperData);
          break;
      case ABLETON_CONNECTED:
          abletonConnected = looperData;
          break;
      case CONFIG:
          enterConfig();
          break;
      default: 
        break;
    }
  
}
void DataLooper::onProgramChange(byte channel, byte program){
  instance = EEPROM.read(0) + program;
  //sendSysEx(0,0,INSTANCE_CHANGE,0);
}

//Teensy only has 1 timer, so handling in base class to avoid duplicates
void DataLooper::blink(){
  blinking = true;
  blinkTimer.begin(endBlink, 50000);
}
void DataLooper::endBlink(){
  blinkTimer.end();
  blinking = false;
}
//TODO on bank change, end timer!!

void DataLooper::altModeCommands(){
  //unsigned char execute_on, unsigned char command_mode, unsigned char action, unsigned char data1, unsigned char data2);
  
  //LOOPER MODE COMMAND DEFAULTS
  for (int x = 0; x<NUM_LOOPERS; x++){
    //BUTTON 1
    loopers[x].buttons[0].commands[0] = getCommand(PRESS_SYSEX, LOOPER_MODE, RECORD, DL_TRACK_TYPE, QUANTIZED, x);
    loopers[x].buttons[0].commands[1] = getCommand(RELEASE_SYSEX, LOOPER_MODE, RECORD, CL_TRACK_TYPE, QUANTIZED, x);
    loopers[x].buttons[0].commands[2] = getCommand(LONG_PRESS_SYSEX, LOOPER_MODE, NEW_CLIP, 0, 0 , x);
    loopers[x].buttons[0].commands[3] = getCommand(PRESS_SYSEX, NEW_SESSION_MODE, RECORD, DL_TRACK_TYPE, UNQUANTIZED, x);
    //BUTTON 2
    loopers[x].buttons[1].commands[0] = getCommand(PRESS_SYSEX, LOOPER_MODE, STOP, BOTH_TRACK_TYPES, QUANTIZED, x );
    loopers[x].buttons[1].commands[1] = getCommand(LONG_PRESS_SYSEX, LOOPER_MODE, CLEAR, BOTH_TRACK_TYPES, 0, x);
    loopers[x].buttons[1].commands[2] = getCommand(PRESS_SYSEX, NEW_SESSION_MODE, STOP, DL_TRACK_TYPE, UNQUANTIZED, x);

    //BUTTON 3
    loopers[x].buttons[2].commands[0] = getCommand(RELEASE_SYSEX, LOOPER_MODE, UNDO, BOTH_TRACK_TYPES, 0, x );
    loopers[x].buttons[2].commands[1] = getCommand(RELEASE_SYSEX, LOOPER_MODE, CHANGE_BANK, ONLY_BANK_WHEN_CLEAR, 0, x );
    loopers[x].buttons[2].commands[2] = getCommand(LONG_PRESS_SYSEX, LOOPER_MODE, CHANGE_BANK, ALWAYS_BANK, 0, x);
  }

  loopers[1].buttons[3].commands[0] = getCommand(PRESS_SYSEX, LOOPER_MODE, MUTE_ALL, BOTH_TRACK_TYPES, TOGGLE, 1);
  loopers[1].buttons[3].commands[1] = getCommand(PRESS_SYSEX, NEW_SESSION_MODE, TAP_TEMPO, NUM_TAPS_BEFORE_METRO, 0, 1 );
  loopers[1].buttons[3].commands[2] = getCommand(DOUBLE_TAP_SYSEX, NEW_SESSION_MODE, TAP_TEMPO, NUM_TAPS_BEFORE_METRO, 0, 1 );

  loopers[2].buttons[3].commands[0] = getCommand(PRESS_SYSEX, LOOPER_MODE, CLEAR_ALL, BOTH_TRACK_TYPES, 2 , 2);
  loopers[2].buttons[3].commands[1] = getCommand(LONG_PRESS_SYSEX, LOOPER_MODE, CHANGE_MODE, NEW_SESSION_MODE, 0, 2);
  loopers[2].buttons[3].commands[2] = getCommand(PRESS_SYSEX, NEW_SESSION_MODE, CHANGE_MODE, LOOPER_MODE, 0 , 2);

  loopers[0].buttons[3].commands[0] = getCommand(PRESS_SYSEX, LOOPER_MODE, TOGGLE_STOP_START, BOTH_TRACK_TYPES, UNQUANTIZED , 0);


  //NEW SESSION MODE COMMANDS
  loopers[0].buttons[0].commands[3] = getCommand(PRESS_SYSEX, NEW_SESSION_MODE, RECORD, BOTH_TRACK_TYPES, UNQUANTIZED , 0);
  loopers[1].buttons[0].commands[3] = getCommand(PRESS_SYSEX, NEW_SESSION_MODE, RECORD, BOTH_TRACK_TYPES, UNQUANTIZED , 1);
  loopers[2].buttons[0].commands[3] = getCommand(PRESS_SYSEX, NEW_SESSION_MODE, RECORD, BOTH_TRACK_TYPES, UNQUANTIZED , 2);

}
DLCommand DataLooper::getCommand(unsigned char execute_on, unsigned char req_mode, unsigned char action, unsigned char data1, unsigned char data2, unsigned char looperNum){
  return DLCommand(execute_on, req_mode, action, data1, data2, looperNum, &mode , &bank, &instance, &(loopers[looperNum].state));

}