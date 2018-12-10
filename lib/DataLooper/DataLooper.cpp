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

DataLooper::DataLooper() {
  blinking = false;
  //initializes new loopers
  loadConfig();
  
}
void DataLooper::init(){
     for (unsigned char i = 0; i < NUM_LOOPERS; i++)
    {
      loopers[i].init(&led_pins[i][0], &control_pins[i][0],i);
    }
    altModeCommands();
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

void DataLooper::toggleNewSession(boolean on){
  // if(on){
  //   for (int l = 0; l < NUM_LOOPERS; l++){
  //     loopers[l].led.setColor(PURPLE);
  //   }
  //   newSessionActive = true;
  // } else{
  //   for (int l = 0; l < NUM_LOOPERS; l++){
  //     if(loopers[l].led.curColor == PURPLE){
  //       loopers[l].led.setColor(WHT);
  //     }
  //   }
  //   newSessionActive = false;
  // }
}
void DataLooper::onSysEx(const uint8_t *sysExData, uint16_t sysExSize, bool complete)
{
  //Translate SYSEX to variable data
  unsigned char looperNum = sysExData[1];
  unsigned char looperCommand = sysExData[2];
  unsigned char looperData = sysExData[4];
  unsigned char reqInstance = looperNum / NUM_LOOPERS;
  unsigned char localLooper = looperNum % NUM_LOOPERS;

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
          Serial.print("State change req");
          Serial.print("\t");
          Serial.print(looperData);
          Serial.println();
          if(reqInstance == instance){
            loopers[localLooper].led.setColor(looperData);
          }
          break;
      case REQUEST_NOTES:
          usbMIDI.sendNoteOn ( ((looperNum ) * NUM_CONTROLS ) + looperData, 127, MIDI_CHAN);
          usbMIDI.sendNoteOff ( ((looperNum ) * NUM_CONTROLS ) + looperData, 127, MIDI_CHAN);
          break;
      case NEW_SESSION:
          toggleNewSession(looperData);
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
  //DLCommand(unsigned char execute_on, unsigned char command_mode, unsigned char action, unsigned char data1, unsigned char data2);
  
  //LOOPER MODE COMMAND DEFAULTS
  // for (int x = 0; x<NUM_LOOPERS; x++){
  //   //BUTTON 1
  //   loopers[x].buttons[0].addCommand(DLCommand(PRESS_SYSEX, LOOPER_MODE, RECORD, DL_TRACK_TYPE, QUANTIZED ));
  //   loopers[x].buttons[0].addCommand(DLCommand(RELEASE_SYSEX, LOOPER_MODE, RECORD, CL_TRACK_TYPE, QUANTIZED ));
  //   loopers[x].buttons[0].addCommand(DLCommand(LONG_PRESS_SYSEX, LOOPER_MODE, NEW_CLIP, 0, 0 ));

  //   //BUTTON 2
  //   loopers[x].buttons[1].addCommand(DLCommand(PRESS_SYSEX, LOOPER_MODE, STOP, BOTH_TRACK_TYPES, QUANTIZED ));
  //   loopers[x].buttons[1].addCommand(DLCommand(LONG_PRESS_SYSEX, LOOPER_MODE, CLEAR, BOTH_TRACK_TYPES, QUANTIZED ));

  //   //BUTTON 3
  //   loopers[x].buttons[2].addCommand(DLCommand(RELEASE_SYSEX, LOOPER_MODE, UNDO, BOTH_TRACK_TYPES, 0 ));
  //   loopers[x].buttons[2].addCommand(DLCommand(RELEASE_SYSEX, LOOPER_MODE, CHANGE_BANK, ONLY_BANK_WHEN_CLEAR, 0 ));
  //   loopers[x].buttons[2].addCommand(DLCommand(LONG_PRESS_SYSEX, LOOPER_MODE, CHANGE_BANK, ALWAYS_BANK, 0 ));

  // }
    cmd = DLCommand(PRESS_SYSEX, LOOPER_MODE, TOGGLE_STOP_START, BOTH_TRACK_TYPES, QUANTIZED );
    loopers[0].buttons[3].addCommand(&cmd);
  // loopers[1].buttons[3].addCommand(DLCommand(PRESS_SYSEX, LOOPER_MODE, MUTE_ALL, BOTH_TRACK_TYPES, TOGGLE ));
  // loopers[2].buttons[3].addCommand(DLCommand(PRESS_SYSEX, LOOPER_MODE, CLEAR_ALL, BOTH_TRACK_TYPES, 0 ));
  // loopers[2].buttons[3].addCommand(DLCommand(LONG_PRESS_SYSEX, LOOPER_MODE, CHANGE_MODE, NEW_SESSION_MODE, 0 ));


  //NEW SESSION MODE COMMANDS
  // loopers[1].buttons[3].addCommand(DLCommand(PRESS_SYSEX, NEW_SESSION_MODE, TAP_TEMPO, NUM_TAPS_BEFORE_METRO, 0 ));
  // loopers[2].buttons[3].addCommand(DLCommand(PRESS_SYSEX, NEW_SESSION_MODE, CHANGE_MODE, LOOPER_MODE, 0 ));
  // loopers[0].buttons[0].addCommand(DLCommand(PRESS_SYSEX, NEW_SESSION_MODE, RECORD, BOTH_TRACK_TYPES, UNQUANTIZED ));
  // loopers[1].buttons[0].addCommand(DLCommand(PRESS_SYSEX, NEW_SESSION_MODE, RECORD, BOTH_TRACK_TYPES, UNQUANTIZED ));
  // loopers[2].buttons[0].addCommand(DLCommand(PRESS_SYSEX, NEW_SESSION_MODE, RECORD, BOTH_TRACK_TYPES, UNQUANTIZED ));

}