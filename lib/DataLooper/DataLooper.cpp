#include "DataLooper.h"

/**
 * @file DataLooper.cpp
 * @author Vince Cimo
 * @version 1.0
 */

//boolean DataLooper::blinking = false;
//IntervalTimer DataLooper::blinkTimer;

DataLooper::DataLooper(WS2812Serial * _leds) {
   leds = _leds;
   //blinking = false;
  
   //initializes buttons
   for(int x = 0; x < NUM_BUTTONS; x++){
     buttons[x].init(BUTTON_PINS[x], x, LED_NUMBERS[x], leds, this);
   }
  loadAltModeCommands();
  loadConfig();
}

void DataLooper::clearControlChanges(unsigned char _ccNum, unsigned char _ccValue){
  for(int x = 0; x < NUM_BUTTONS; x++){
    buttons[x].clearControlChanges(_ccNum, _ccValue);
  }
}

 void DataLooper::loadConfig(){

   // CHANNEL
  //  if(EEPROM.read(126) == 255){
  //    //channel 14 default
  //    EEPROM.write(126,14);
  //    channel = 14;
  //  } else{
	//    channel = EEPROM.read(126);
  //  }

   // DEFAULT MODE ON STARTUP
    State::mode = 0;
  //  if(EEPROM.read(125) == 255){
  //    //looper mode default
  //    EEPROM.write(125,0);
     
  //  } else{
	//    State::mode = EEPROM.read(125);
  //  }



   //EEPROM BUTTON COMMANDS LOADED IN INDIVIDUAL CLASS
 }

void DataLooper::loadAltModeCommands(){
  
  
}
void DataLooper::scanForButtonActivity(unsigned long current_time){
  checkForModeChange();
    if(State::blinking && blinkTimer > BLINK_TIME){
      for (unsigned char i = 0; i < NUM_BUTTONS; i++)
        {
          buttons[i].unBlink();
        }
      State::blinking = false;
   }
    if(State::inConfig == 2 && State::configExitMillis >= 1000){
      leds->clear();
      leds->show();
      State::inConfig = 0;
      State::configExitMillis = 0;
    }
    for (unsigned char i = 0; i < NUM_BUTTONS; i++)
    {
      buttons[i].update(current_time);
    }
}

// void DataLooper::checkForBankChange(){
//   // if (State::bank != State::lastBank){
//   //   for (unsigned char i = 0; i < NUM_BUTTONS; i++)
//   //   {
//   //     buttons[i].setColor(WHT);
//   //   }
//   //   State::lastBank = State::bank;
//   // }
// }

void DataLooper::checkForModeChange(){
  if (State::mode != State::lastMode){
    Serial.println("checking modeeee");
    changeMode();
    State::lastMode = State::mode;
    byte array[] = {0x1E, 8, State::mode, 0, 0, 0};
    usbMIDI.sendSysEx(6, array, false);
  }
}
void DataLooper::changeMode(){
  for(int x = 0; x< NUM_BUTTONS; x++){
    buttons[x].changeMode();
  }
}

// void DataLooper::changeBank(unsigned char newBank){
//   State::bank = newBank;
// }

void DataLooper::onSysEx(const uint8_t *sysExData, uint16_t sysExSize, bool complete)
{
  //Translate SYSEX to variable data
  if(sysExData[1] == 0x1e){
    switch(sysExData[2]){
      case 0:
        //DAW Connection
        if(sysExData[3] == 0x01){
          State::dawConnected = true;
          Serial.print("daw connected");
        } else{
          State::dawConnected = false;
          Serial.print("daw disconnected");
        }
        break;      
      case 1:
        //state change
        //Serial.print("looper #:");
        //Serial.println(sysExData[3]);
        //Serial.print("state:");
        //Serial.print(sysExData[4]);
        for(int x=0; x<NUM_BUTTONS; x++){
          buttons[x].onLooperStateChange(sysExData[3], sysExData[4]);
        }
        break;
      case 2:
        Serial.println("looper command requested");
        usbMIDI.sendNoteOn(sysExData[4],127,14);
        usbMIDI.sendNoteOff(sysExData[4],127,14);
        break;
      case 3:
        for(int x=0; x<NUM_BUTTONS; x++){
          buttons[x].onBeat(sysExData[3]);
        }
         State::blinking = true;
         blinkTimer = 0;
        break;
      case 7:
        Serial.println("in config");
        State::inConfig = 1;
        break;
      case 8:
        Serial.println("config byte");
        configureDL(sysExData);
        break;
      case 9:
        Serial.println("end config");
        for(int x=0; x<NUM_BUTTONS; x++){
          buttons[x].loadCommands();
          leds->setPixel(LED_NUMBERS[x], RED.rgb);
          leds->show();
          State::inConfig = 2;
          State::configExitMillis = 0;
        }
        break;
      default:
        break;

    }
  }
}

void DataLooper::offBeat(){
  for(int x = 0; x < NUM_BUTTONS; x++){
    buttons[x].restoreColor();
  }
}

void DataLooper::configureDL(const uint8_t *sysExData){
      ee_storage_typ command;
      Serial.print("Command #: ");
      Serial.println(sysExData[sysExStartByte-1]);
          for(int byte = 0; byte < SYSEX_BYTES_PER_COMMAND; byte++){
            switch(byte){
              case 0:
                Serial.print("Button Action: ");
                command.commands.button_action = sysExData[sysExStartByte+byte];
                break;
              case 1:
                Serial.print("Action: ");
                command.commands.action = sysExData[sysExStartByte+byte];
                break;
              case 2:
                Serial.print("Data 1: ");
                command.commands.data1 = sysExData[sysExStartByte+byte];
                break;
              case 3:
                Serial.print("Data 2: ");
                command.commands.data2 = sysExData[sysExStartByte+byte];
                break;
              case 4:
                Serial.print("Data 3: ");
                command.commands.data3 = sysExData[sysExStartByte+byte];
                break;
              case 5:
                Serial.print("Data 4: ");
                command.commands.data4 = sysExData[sysExStartByte+byte];
                break;
              case 6:
                Serial.print("Data 5: ");
                command.commands.data5 = sysExData[sysExStartByte+byte];
                break;  
              case 7:
                Serial.print("LED Control: ");
                command.commands.led_control = sysExData[sysExStartByte+byte];
                break;
              case 8:
                Serial.print("Mode: ");
                command.commands.mode = sysExData[sysExStartByte+byte];
                break;
            }
            Serial.println (sysExData[sysExStartByte+byte]);
            
          } 
  writeCommand(sysExData[sysExStartByte-1], command);
}

void DataLooper::writeCommand(uint8_t actionNum, ee_storage_typ command){
  int curByte = 0;
  int startByte = actionNum * BYTES_PER_COMMAND;
  for(int byte = 0; byte < BYTES_PER_COMMAND; byte++){
      EEPROM.write(startByte+curByte, command.asBytes[byte]);
      curByte++;
    }
}

void DataLooper::onProgramChange(byte channel, byte program){
  //State::instance = EEPROM.read(0) + program;
  //sendSysEx(0,0,INSTANCE_CHANGE,0);
}

// //Teensy only has 1 timer, so handling in base class to avoid duplicates
// void DataLooper::blink(){
//   blinking = true;
//   blinkTimer.begin(endBlink, 50000);
// }
// void DataLooper::endBlink(){
//   blinkTimer.end();
//   blinking = false;
// }
// //TODO on bank change, end timer!!


