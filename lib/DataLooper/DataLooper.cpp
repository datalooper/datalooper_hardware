#include <Arduino.h>
#include "DataLooper.h"
#include <EEPROM.h>

/**
 * @file DataLooper.cpp
 * @author Vince Cimo
 * @version 1.0
 */

int DataLooper::instance;
int DataLooper::looperNum;
int DataLooper::looperCommand;
boolean DataLooper::configActive;
boolean DataLooper::newSessionActive;
unsigned long DataLooper::configOffDelay;
boolean DataLooper::onBeat;
unsigned long DataLooper::beat_time;
int DataLooper::led_pins[NUM_LOOPERS][LED_PINS] = {{3, 4, 6}, {9, 10, 16}, {17, 20, 22}};
int DataLooper::control_pins[NUM_LOOPERS][NUM_CONTROLS] = {{0, 1, 2, 5}, {7, 8, 11, 12}, {13, 14, 15, 18}};

//initializes new loopers
Looper* DataLooper::loopers[NUM_LOOPERS] = {new Looper(), new Looper(),new Looper()};

DataLooper::DataLooper() {

}

void DataLooper::init(){
  //SYSEX STUFF
	looperNum = 0;
	looperCommand = 0;

	//STATE VARIABLES
	configActive = false;
	newSessionActive = false;
	configOffDelay = -1;
	onBeat = false;
	beat_time = 0;
  
  //initializes new loopers
  for (int i = 0; i < NUM_LOOPERS; i++){
    loopers[i]->configureLooper(i, instance, led_pins[i], control_pins[i]);
  }
}
void DataLooper::loadConfig(){
	//Checks for stored config value
	instance = EEPROM.read(CONFIG_ADDRESS);
	if(instance == 255){
		EEPROM.write(CONFIG_ADDRESS, 0);
	}
}

void DataLooper::checkIfOnBeat(long current_time){
  if(onBeat){
    beat_time = current_time;
    onBeat = false;
  }
}

//TODO Move code to LED class, blink method should just call blink, also, rather than storing beat_time, just start 50ms timer onBeat sysex
void DataLooper::blinkLEDs(long current_time){
  if (current_time - beat_time >= BLINK_TIME) {
    for (int i = 0; i < NUM_LOOPERS; i++)
    {
      loopers[i]->led->restoreColor();
    }
  } else if(!configActive && !newSessionActive) {
    for (int i = 0; i < NUM_LOOPERS; i++)
    {
      if ( i == instance % NUM_LOOPERS){
        loopers[i]->led->writeColor(NONE);
      }
    }
  }
}

void DataLooper::checkConfig(long current_time){
  //Turns LEDs back to white after config exited
  if(configActive &&  configOffDelay != -1 && current_time - configOffDelay >= 2000){
        for( int x = 0; x < NUM_LOOPERS; x++){
          loopers[x]->led->setColor(WHT);
          configActive = false;
        }
    configOffDelay = -1;
  }
}

void DataLooper::scanForButtonActivity(long current_time){
    for (int i = 0; i < NUM_LOOPERS; i++)
    {
      for (int n = 0; n < NUM_CONTROLS; n++) {
        loopers[i]->buttons[n].update();

        //Buttons accidentely backwards on loop 3 in v1.1; here's the software fix; oops.
        if ((i == NUM_LOOPERS-1 && loopers[i]->buttons[n].fallingEdge()) || ( i != NUM_LOOPERS-1 && loopers[i]->buttons[n].risingEdge()))
        {
          //On press
          if (loopers[i]->press_time[n] != -1 &&  current_time - loopers[i]->press_time[n] <= DOUBLE_HIT_TIME){
            onMultiPress(i, n, current_time);
          } else{
            onButtonPress(i, n, current_time);
          }
        }
        else if ((i == NUM_LOOPERS-1 && loopers[i]->buttons[n].risingEdge()) || ( i != NUM_LOOPERS-1 && loopers[i]->buttons[n].fallingEdge()))
        {
          //On release
          if (loopers[i]->press_time[n] == -1){
            onMultiRelease(i, n, current_time);
          } else{
            onButtonRelease(i, n, current_time);
          }
        }  

        //checks for a long press
        if(loopers[i]->is_pressed[n] && current_time - loopers[i]->press_time[n] >= loopers[i]->long_press_time[n]){
            onLongPress(i, n, loopers[i]->long_press_time[n]);
        }
      }    
    }
}
void DataLooper::onMultiPress(int looper, int control, long current_time){
  loopers[looper]->press_time[control] = -1;
  sendSysEx(looper, control, MULTI_TAP, 0);
}
void DataLooper::onMultiRelease(int looper, int control, long current_time){
  sendSysEx(looper, control, MULTI_RELEASE, 0);
  loopers[looper]->is_pressed[control] = false;
  loopers[looper]->long_press_time[control] = 500;
}
void DataLooper::onButtonPress(int looper, int control, long current_time){
  //diagnoseButton(i,n, loopers[i]->ccs[n]);
  //Marks time that button was initially pressed
  loopers[looper]->press_time[control] = current_time;
  loopers[looper]->is_pressed[control] = true; 
  Serial.print("Sending button press. Looper #");
  Serial.print(looper);
  Serial.print(" Control #");
  Serial.print(control);
  Serial.println();
  if(configActive){
    configureLoopers(looper, control);
  } else{
    sendSysEx(looper, control, BUTTON_PRESS, 0);
  }
  //usbMIDI.sendNoteOn (loopers[looper]->ccs[control], 127, MIDI_CHAN);
}

void DataLooper::onButtonRelease(int looper, int control, long current_time){

  Serial.print("Sending button release");
  Serial.println();
  if(!configActive){
    sendSysEx(looper, control, BUTTON_RELEASE, loopers[looper]->long_press_time[control] / 500 - 1 );
  }
  loopers[looper]->is_pressed[control] = false;
  loopers[looper]->long_press_time[control] = 500;
  //usbMIDI.sendNoteOff (loopers[looper]->ccs[control], 127, MIDI_CHAN);
}

void DataLooper::onLongPress(int looper, int control, int long_press_time){
  Serial.print("Sending long press");
  Serial.println();
  Serial.print(long_press_time);
  Serial.println();
  if(!configActive){
    sendSysEx(looper, control, LONG_PRESS, long_press_time / 500);
  }
  loopers[looper]->long_press_time[control] += 500;
  //usbMIDI.sendNoteOn (loopers[looper]->ccs[control] + (NUM_LOOPERS * NUM_CONTROLS), 127, MIDI_CHAN);
}

void DataLooper::enterConfig(){
  configActive = true;
  Serial.print("Entering config");
  for (int l = 0; l < NUM_LOOPERS; l++){
    loopers[l]->led->setColor(CYAN);
  }
}
void DataLooper::configureLoopers(int i,int n){
  instance = ( i * NUM_CONTROLS) + n;
  Serial.print("configuring looper as: ");
  Serial.print(instance);
  EEPROM.write(CONFIG_ADDRESS, instance);
  for( int x = 0; x < NUM_LOOPERS; x++){
    loopers[x]->configureLooper(x, instance, led_pins[x], control_pins[x]);
    loopers[x]->led->setColor(GREEN);
    configOffDelay = millis();
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
  if(on){
    for (int l = 0; l < NUM_LOOPERS; l++){
      loopers[l]->led->setColor(PURPLE);
    }
    newSessionActive = true;
  } else{
    for (int l = 0; l < NUM_LOOPERS; l++){
      if(loopers[l]->led->curColor == PURPLE){
        loopers[l]->led->setColor(WHT);
      }
    }
    newSessionActive = false;
  }
}
void DataLooper::onSysEx(const uint8_t *sysExData, uint16_t sysExSize, bool complete)
{
  //Translate SYSEX to variable data
  looperNum = sysExData[1];
  looperCommand = sysExData[2];
  int looperData = sysExData[4];
  int reqInstance = looperNum / NUM_LOOPERS;
  int localLooper = looperNum % NUM_LOOPERS;

  switch (looperCommand)
    {
      case RESET:
          toggleNewSession(false);
          for( int x = 0; x < NUM_LOOPERS; x++){
            loopers[x]->led->setColor(WHT);
          }
      case DOWNBEAT:
          onBeat = true;
          break;
      case CHANGE_STATE:
          Serial.print("State change req");
          Serial.print("\t");
          Serial.print(looperData);
          Serial.println();
          if(reqInstance == instance){
            loopers[localLooper]->led->setColor(looperData);
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
  instance = EEPROM.read(CONFIG_ADDRESS) + program;
  sendSysEx(0,0,INSTANCE_CHANGE,0);
}
void DataLooper::sendSysEx(int looper, int control, byte action, byte long_press_seconds){
  byte sending = 0x12;
  byte array[] = {0x41, (byte) instance, (byte) looper, (byte) control, sending, action, long_press_seconds, 0xF7};
  usbMIDI.sendSysEx(7, array, false);
}

