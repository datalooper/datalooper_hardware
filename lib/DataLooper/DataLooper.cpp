#include "DataLooper.h"

/**
 * @file DataLooper.cpp
 * @author Vince Cimo
 * @version 1.0
 */
MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI);

DataLooper::DataLooper(WS2812Serial * _leds) {
   leds = _leds;
   //blinking = false;
  
   //initializes buttons
   for(int x = 0; x < NUM_BUTTONS; x++){
     buttons[x].init(BUTTON_PINS[x], x, LED_NUMBERS[x], leds, this);
   }
  
}

void DataLooper::clearControlChanges(unsigned char _ccNum, unsigned char _ccValue){
  for(int x = 0; x < NUM_BUTTONS; x++){
    buttons[x].clearControlChanges(_ccNum, _ccValue);
  }
}

void DataLooper::beginMIDI(){
    MIDI.begin();
}

void DataLooper::onPresetChange(){

  //Order of operations important here, hence the multiple loops
  for(int x = 0; x < NUM_BUTTONS; x++){
    Serial.print("in loadCommand :");
    Serial.println(x);
    buttons[x].led.setColor(NONE);
    buttons[x].loadCommands();
  }
  loadAltModeCommands();
  loadConfig();

  byte array[] = {DATALOOPER_IDENTIFIER, ACTIONS.CHANGE_INSTANCE};
  usbMIDI.sendSysEx(8, array, false);
  bool shouldRequestRebuild = false;
  for(int x = 0; x< NUM_BUTTONS; x++){
        buttons[x].requestState();
        shouldRequestRebuild = buttons[x].shouldRequestRebuild();
  }
  if(shouldRequestRebuild){
    Serial.println("requesting rebuild");
    byte array[] = {DATALOOPER_IDENTIFIER, ACTIONS.REQUEST_MIDI_MAP_REBUILD};
    usbMIDI.sendSysEx(8, array, false);    
  }
}
 void DataLooper::loadConfig(){
    State::globalChannel = readByte(GLOBAL_SETTINGS.GLOBAL_CHANNEL);
    State::mode = readByte(GLOBAL_SETTINGS.STARTING_MODE);
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
      ee_storage.commands.data3 = 0;
      ee_storage.commands.data4 = x/4 + 1;
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
  if(State::inConfig == 3 && State::configExitMillis >= 1000){
     Serial.print("it made it");
      leds->clear();
      leds->show();
      State::inConfig = 0;
      State::configExitMillis = 0;
      State::presetsWritten = 0;
      onPresetChange();
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
    byte array[] = {DATALOOPER_IDENTIFIER, ACTIONS.CHANGE_MODE, State::mode, 0, 0, 0};
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

void DataLooper::onSysEx(const uint8_t *sysExData, uint16_t sysExSize, bool complete)
{
  //Translate SYSEX to variable data
  if(sysExData[1] == DATALOOPER_IDENTIFIER){
    switch(sysExData[2]){
      case SYSEX_COMMANDS.DAW_CONNECTION:
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
      case SYSEX_COMMANDS.STATE_CHANGE:
        //state change
        Serial.print("updating state on button:");
        Serial.print(sysExData[3]);
        Serial.print(" state:");
        Serial.println(sysExData[4]);
        if(State::mode != MODES.CLIP_LAUNCH_MODE){
          buttons[sysExData[3]].fastBlink(false);
          buttons[sysExData[3]].updateLooperState(sysExData[4]);
        }
        break;
      case SYSEX_COMMANDS.REQUEST_MIDI:
        Serial.println("looper command requested");
        for(int x=0; x<NUM_BUTTONS; x++){
            if(buttons[x].onLooperStateChange(sysExData[3])){
              //If not undo command, indicate fast blink for requested command
              Serial.print("cmd request");
              Serial.println(sysExData[4] - sysExData[3] * 4);
              if(sysExData[4] - sysExData[3] * 4 != 2 && !(buttons[x].led.curColor.rgb == WHT.rgb && sysExData[4] - sysExData[3] * 4 == 3)){
                buttons[x].fastBlink(true);
              }
            }
          }
        usbMIDI.sendNoteOn(sysExData[4],127, State::globalChannel);
        usbMIDI.sendNoteOff(sysExData[4],127, State::globalChannel);
        break;
      case SYSEX_COMMANDS.ON_BEAT:
        if(State::mode != MODES.CLIP_LAUNCH_MODE){
          for(int x=0; x<NUM_BUTTONS; x++){
            buttons[x].onBeat(sysExData[3]);
          }
          State::blinking = true;
          blinkTimer = 0;
        }
        break;
      case SYSEX_COMMANDS.CHANGE_COLOR:
          mapColors(sysExData[3], (sysExData[4] << 1)  ^ sysExData[5], (sysExData[6] << 1) ^ sysExData[7], (sysExData[8] << 1) ^ sysExData[9] );
          break;
      case SYSEX_COMMANDS.CHANGE_BLINK:
        buttons[sysExData[3]].toggleBlink(sysExData[4]);
        break;
      case SYSEX_COMMANDS.START_CONFIG:
        Serial.println("in config");
        State::inConfig = 1;
        for(int x=0; x<NUM_BUTTONS; x++){
          buttons[x].startConfig();
        } 
        break;
      case SYSEX_COMMANDS.CONFIG_ACTION_BYTE:
        Serial.print("config byte on button #");
        Serial.println(sysExData[3]);
        buttons[sysExData[3]].configureDL(sysExData);
        break;
      case SYSEX_COMMANDS.CONFIG_MIDI_SENT:
          State::inConfig = 2;
          for(int x = 0; x<NUM_BUTTONS; x++){
            for(int y = 0; y < NUMBER_USER_COMMANDS; y++){
              writeCommand(x, y, buttons[x].commands[y]);
            }
          }
          if(sysExData[3] == 1){
            endConfig();
          } else{
            State::presetsWritten += 1;
            Serial.print("requesting preset: ");
            Serial.println(State::presetsWritten + 1);
            byte array[] = {DATALOOPER_IDENTIFIER, 12, State::presetsWritten + 1, 0, 0, 0};
            usbMIDI.sendSysEx(6, array, false);
          }
          
          Serial.println("midi all sent");
        break;
      case SYSEX_COMMANDS.CONFIG_GLOBAL_BYTE:
        State::presetsWritten = 0;
        globalCommands[sysExData[3]] = sysExData[4];
        Serial.println("receiving config global byte");
        break;
      case SYSEX_COMMANDS.GLOBAL_SETTINGS_SENT:
        writeGlobalConfig();
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
void DataLooper::requestState(){
    for(int x = 0; x < NUM_BUTTONS; x++){
    buttons[x].requestState();
  }
}
void DataLooper::onProgramChange(byte channel, byte program){
  Serial.println("changing preset to #");
  Serial.println(program);
  State::preset = program;
  onPresetChange();
}
void DataLooper::onControlChange(uint8_t channel, uint8_t control, uint8_t value){
  for(int x = 0; x < NUM_BUTTONS; x++){
    buttons[x].onControlChange(channel, control, value);
  }
}

void DataLooper::sendNoteOn(uint8_t inNoteNumber,uint8_t inVelocity, uint8_t inChannel){
  Serial.println("sending note on MIDI port");
  MIDI.sendNoteOn(inNoteNumber,inVelocity,inChannel);
}
void DataLooper::sendNoteOff(uint8_t inNoteNumber, uint8_t inVelocity, uint8_t inChannel){
  Serial.println("sending note off MIDI port");
  MIDI.sendNoteOff(inNoteNumber, inVelocity, inChannel);
}
void DataLooper::sendProgramChange(uint8_t inProgramNumber, uint8_t inChannel){
  Serial.println("sending pc MIDI port");
  MIDI.sendProgramChange(inProgramNumber,inChannel);
 }
void DataLooper::sendControlChange(uint8_t inControlNumber, uint8_t inControlValue, uint8_t inChannel){
  Serial.println("sending cc on MIDI port");
  MIDI.sendControlChange(inControlNumber, inControlValue, inChannel);
                          }
void DataLooper::sendSysEx(unsigned inLength, const byte* inArray, bool inArrayContainsBoundaries){
  //Serial.println("sending sysex on MIDI port");
  //MIDI.sendSysEx(inLength, inArray, inArrayContainsBoundaries);                    
}
             

void DataLooper::endConfig(){
  for(int x=0; x<NUM_BUTTONS; x++){
          leds->setPixel(LED_NUMBERS[x], GREEN.rgb);
          leds->show();
        }
  State::configExitMillis = 0;
  State::inConfig = 3;
}

void DataLooper::writeGlobalConfig(){
  for( int x = 0; x < NUM_GLOBAL_CONFIG_BYTES; x++){
      int oldByte = readByte(x);
      if( oldByte != globalCommands[x]){
        Wire.beginTransmission(0x50);
        Wire.write(x); // LSB
        Wire.write(globalCommands[x]);
        Wire.endTransmission();
        delay(10);
      }
  }
  //Request presets
  byte array[] = {DATALOOPER_IDENTIFIER, 12, 1, 0, 0, 0};
  usbMIDI.sendSysEx(6, array, false);
}
void DataLooper::writeCommand(uint8_t buttonNumber, uint8_t commandNum, DLCommand command){
  int startByte = (buttonNumber * NUMBER_USER_COMMANDS * BYTES_PER_COMMAND) +  commandNum * BYTES_PER_COMMAND + (State::presetsWritten * NUM_BUTTONS * NUMBER_USER_COMMANDS * BYTES_PER_COMMAND) + NUM_GLOBAL_CONFIG_BYTES;
//   command->waitingForWrite = false;
//   State::EEPROMWriting = 0;

  for(int byte = 0; byte < BYTES_PER_COMMAND; byte++){   
    int eeaddress = (startByte + byte );
    int daddr = 0x50 | ((eeaddress >> 8) );
    int oldByte = readByte(eeaddress);
    // Serial.print("Writing button #");
    //         Serial.print(buttonNumber);
    //         Serial.print(" Command #");
    //         Serial.print(commandNum);
    //         Serial.print(" at address: ");
    //         Serial.println(startByte);    
    // Serial.print("old byte:");
    // Serial.print(oldByte);
    // Serial.print(" new byte: ");
    // Serial.println(command.ee_storage.asBytes[byte]);
    
    if(command.ee_storage.asBytes[byte] != oldByte){      
      Wire.beginTransmission(daddr);
      Wire.write(eeaddress & 0xff); // LSB
      Wire.write(command.ee_storage.asBytes[byte]);
      Wire.endTransmission();
      delay(10);
      // Serial.print("writing byte:");
      // Serial.print(command.ee_storage.asBytes[byte]);
      // Serial.print(" at addr:");
      // Serial.print(daddr);
      // Serial.print(" at byte addr: ");
      // Serial.println(eeaddress & 0xff);
      
      
    }
//   }
}
}

//TODO METHOD DUPLICATED IN BUTTON.CPP
byte DataLooper::readByte(unsigned int eeaddress ) {
    byte rdata = 0x7F;
    int daddr = 0x50 | ((eeaddress >> 8) );

    Wire.beginTransmission(daddr);
    Wire.write(eeaddress & 0xff); // LSB
    Wire.endTransmission();
    Wire.requestFrom(daddr,1);
    if (Wire.available()) { rdata = Wire.read(); 
        // Serial.print("reading byte:");
        // Serial.print(rdata);
        // Serial.print(" at addr:");
        // Serial.print(daddr);
        // Serial.print(" at byte addr: ");
        // Serial.println(eeaddress & 0xff);
    }
    else{
        Serial.print("wire not available at addr:");
        Serial.println(eeaddress);
    }
    
    return rdata;
}