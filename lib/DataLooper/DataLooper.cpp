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
  bank = 0;
  int led_pins[NUM_LOOPERS][LED_PINS] = {{3, 4, 6}, {9, 10, 16}, {17, 20, 22}};
  int control_pins[NUM_LOOPERS][NUM_CONTROLS] = {{0, 1, 2, 5}, {7, 8, 11, 12}, {13, 14, 15, 18}};
  //const Looper loopers[NUM_LOOPERS] = { Looper(&led_pins[0][0], &control_pins[0][0],0), Looper(&led_pins[1][0], &control_pins[1][0],1), Looper(&led_pins[2][0], &control_pins[2][0],2)};

  int current_mode = LOOPER_MODE;
  blinking = false;
  //initializes new loopers
  //loadConfig();
  
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
  current_mode = EEPROM.read(125);
  if(current_mode == 255){
    //looper mode default
    EEPROM.write(125,0);
  }

  //EEPROM BUTTON COMMANDS LOADED IN INDIVIDUAL CLASS
}

void DataLooper::scanForButtonActivity(long current_time){
    for (int i = 0; i < NUM_LOOPERS; i++)
    {
      loopers[i].led.setColor(PURPLE);
      //loopers[i].updateButtons(current_time);  
      // if(blinking){
      //     Serial.print("blink");
      //     Serial.println();
      //     //loopers[i].led.writeColor(GREEN);
      // } else{
      //   //loopers[i].led.restoreColor();
      // }
    }
}

void DataLooper::enterConfig(){
  Serial.print("Entering config");
  for (int l = 0; l < NUM_LOOPERS; l++){
    loopers[l].led.setColor(CYAN);
  }
}
void DataLooper::configureLoopers(int i,int n){
  instance = ( i * NUM_CONTROLS) + n;
  Serial.print("configuring looper as: ");
  Serial.print(instance);
  EEPROM.write(0, instance);
  for( int x = 0; x < NUM_LOOPERS; x++){
    // loopers[x].configureLooper(led_pins[x], control_pins[x], x);
    loopers[x].led.setColor(GREEN);
    //configOffDelay = millis();
  }
  sendSysEx(127,127,127,127);
}
void DataLooper::diagnoseButton(int i, int n, int num){
  Serial.print("looper:");
  Serial.print( i );
  Serial.println();
  Serial.print("button: ");
  Serial.print(n);
  Serial.println();
  Serial.print("cc#: ");
  Serial.print(num);
  Serial.println();
}

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
  int looperNum = sysExData[1];
  int looperCommand = sysExData[2];
  int looperData = sysExData[4];
  int reqInstance = looperNum / NUM_LOOPERS;
  int localLooper = looperNum % NUM_LOOPERS;

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
      // case CHANGE_STATE:
      //     Serial.print("State change req");
      //     Serial.print("\t");
      //     Serial.print(looperData);
      //     Serial.println();
      //     if(reqInstance == instance){
      //       loopers[localLooper].led.setColor(looperData);
      //     }
      //     break;
      // case REQUEST_NOTES:
      //     usbMIDI.sendNoteOn ( ((looperNum ) * NUM_CONTROLS ) + looperData, 127, MIDI_CHAN);
      //     usbMIDI.sendNoteOff ( ((looperNum ) * NUM_CONTROLS ) + looperData, 127, MIDI_CHAN);
      //     break;
      // case NEW_SESSION:
      //     toggleNewSession(looperData);
      //     break;
      // case CONFIG:
      //     enterConfig();
      //     break;
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
