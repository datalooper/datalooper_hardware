#include "DataLooper.h"

/**
 * @file DataLooper.cpp
 * @author Vince Cimo
 * @version 1.0
 */

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
  
  DLCommand tapTempo;
  tapTempo.ee_storage.commands.action = SYSEX;
  tapTempo.ee_storage.commands.button_action = BUTTON_ACTIONS.PRESS;
  tapTempo.ee_storage.commands.data1 = ACTIONS.TAP_TEMPO;
  tapTempo.ee_storage.commands.mode = MODES.NEW_SESSION_MODE;
  
  DLCommand exitMode;
  exitMode.ee_storage.commands.action = DATALOOPER_SPECIFIC;
  exitMode.ee_storage.commands.button_action = BUTTON_ACTIONS.PRESS;
  exitMode.ee_storage.commands.data1 = DLACTIONS.CHANGE_MODE;
  exitMode.ee_storage.commands.mode = MODES.ALL_BUT_USER;
  exitMode.ee_storage.commands.data2 = MODES.USER_MODE;

  DLCommand moveSHUp;
  moveSHUp.ee_storage.commands.action = SYSEX;
  moveSHUp.ee_storage.commands.button_action = BUTTON_ACTIONS.RELEASE;
  moveSHUp.ee_storage.commands.data1 = ACTIONS.MOVE_SESSION_HIGHLIGHT;
  moveSHUp.ee_storage.commands.data2 = 0;
  moveSHUp.ee_storage.commands.mode = MODES.CLIP_LAUNCH_MODE;

  DLCommand moveSHLeft;
  moveSHLeft.ee_storage.commands.action = SYSEX;
  moveSHLeft.ee_storage.commands.button_action = BUTTON_ACTIONS.LONG_PRESS;
  moveSHLeft.ee_storage.commands.data1 = ACTIONS.MOVE_SESSION_HIGHLIGHT;
  moveSHLeft.ee_storage.commands.data2 = 2;
  moveSHLeft.ee_storage.commands.mode = MODES.CLIP_LAUNCH_MODE;

  DLCommand moveSHDown;
  moveSHDown.ee_storage.commands.action = SYSEX;
  moveSHDown.ee_storage.commands.button_action = BUTTON_ACTIONS.RELEASE;
  moveSHDown.ee_storage.commands.data1 = ACTIONS.MOVE_SESSION_HIGHLIGHT;
  moveSHDown.ee_storage.commands.data2 = 1;
  moveSHDown.ee_storage.commands.mode = MODES.CLIP_LAUNCH_MODE;

  DLCommand moveSHRight;
  moveSHRight.ee_storage.commands.action = SYSEX;
  moveSHRight.ee_storage.commands.button_action = BUTTON_ACTIONS.LONG_PRESS;
  moveSHRight.ee_storage.commands.data1 = ACTIONS.MOVE_SESSION_HIGHLIGHT;
  moveSHRight.ee_storage.commands.data2 = 3;
  moveSHRight.ee_storage.commands.mode = MODES.CLIP_LAUNCH_MODE;

  buttons[EXIT_BUTTON_NUM].commands[2] = exitMode;

  buttons[TAP_TEMPO_BUTTON_NUM].commands[2] = tapTempo;
  buttons[TAP_TEMPO_BUTTON_NUM].commands[3] = moveSHUp;
  buttons[TAP_TEMPO_BUTTON_NUM].commands[4] = moveSHLeft;

  buttons[11].commands[3] = moveSHDown;
  buttons[11].commands[4] = moveSHRight;

  for(int x = 0; x < NUM_BUTTONS; x++){
    if(x != 3 && x != 7 && x != 11){
      ee_storage_typ ee_storage;
      ee_storage.commands.action = SYSEX;
      ee_storage.commands.button_action = BUTTON_ACTIONS.PRESS;
      ee_storage.commands.data1 = ACTIONS.FIXED_CLIP_CONTROL;
      ee_storage.commands.data2 = x % 4 + 1;
      ee_storage.commands.data3 = 4;
      ee_storage.commands.data4 = 1;
      ee_storage.commands.data5 = x/4 + 1;
      ee_storage.commands.mode = MODES.CLIP_LAUNCH_MODE;
      buttons[x].addCommand(ee_storage, 3);
    }
  }
  Serial.println("loading alt commands");
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
      for (unsigned char i = 0; i < NUM_BUTTONS; i++)
      {
        buttons[i].requestState();
      }
    }
    for (unsigned char i = 0; i < NUM_BUTTONS; i++)
    {
      buttons[i].update(current_time);
    }
}

void DataLooper::checkForModeChange(){
  if (State::mode != State::lastMode){
    changeMode();
    State::lastMode = State::mode;
    byte array[] = {0x1E, ACTIONS.CHANGE_MODE, State::mode, 0, 0, 0};
    usbMIDI.sendSysEx(6, array, false);

    if(State::mode == MODES.CLIP_LAUNCH_MODE){
      State::blinking = false;
    }
    for (unsigned char i = 0; i < NUM_BUTTONS; i++)
      {
        buttons[i].requestState();
      }
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
          Serial.println("daw connected");
          for(int x = 0; x<NUM_BUTTONS; x++){
            buttons[x].requestState();
          }
        } else{
          State::dawConnected = false;
          Serial.print("daw disconnected");
        }
        break;      
      case 1:
        //state change
        Serial.print("state:");
        Serial.println(sysExData[4]);
        if(State::mode != MODES.CLIP_LAUNCH_MODE){
          buttons[sysExData[3]].fastBlink(false);
          buttons[sysExData[3]].updateLooperState(sysExData[4]);
        }
        break;
      case 2:
        Serial.println("looper command requested");
        for(int x=0; x<NUM_BUTTONS; x++){
            if(buttons[x].onLooperStateChange(sysExData[3])){
              //If not undo command, indicate fast blink for requested command
              if(sysExData[4] - sysExData[3] * 4 != 2){
                buttons[x].fastBlink(true);
              }
            }
          }
        usbMIDI.sendNoteOn(sysExData[4],127,14);
        usbMIDI.sendNoteOff(sysExData[4],127,14);
        break;
      case 3:
        if(State::mode != MODES.CLIP_LAUNCH_MODE){
          for(int x=0; x<NUM_BUTTONS; x++){
            buttons[x].onBeat(sysExData[3]);
          }
          State::blinking = true;
          blinkTimer = 0;
        }
        break;
      case 5:
          mapColors(sysExData[3], (sysExData[4] << 1)  ^ sysExData[5], (sysExData[6] << 1) ^ sysExData[7], (sysExData[8] << 1) ^ sysExData[9] );
          break;
      case 6:
        buttons[sysExData[3]].toggleBlink(sysExData[4]);
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
        delay(500);
        for(int x=0; x<NUM_BUTTONS; x++){
          buttons[x].loadCommands();
          leds->setPixel(LED_NUMBERS[x], RED.rgb);
          leds->show();
          State::inConfig = 2;
          State::configExitMillis = 0;
        }
        loadAltModeCommands();
        break;
      default:
        break;

    }
  }
}
void DataLooper::mapColors(unsigned char buttonNum, unsigned char red, unsigned char green, unsigned char blue){
        colorType color;
        color.colors.red = red * .2;
        color.colors.green = green * .2;
        color.colors.blue = blue * .2;
        buttons[buttonNum].led.setColor(color);
}
void DataLooper::offBeat(){
  for(int x = 0; x < NUM_BUTTONS; x++){
    buttons[x].led.restoreColor();
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
void DataLooper::onControlChange(uint8_t channel, uint8_t control, uint8_t value){
  for(int x = 0; x < NUM_BUTTONS; x++){
    buttons[x].onControlChange(channel, control, value);
  }
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


