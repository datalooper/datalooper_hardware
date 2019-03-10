#ifndef DLOBSERVER
#define DLOBSERVER

class DLObserver {
    public:
        virtual void clearControlChanges(unsigned char _ccNum, unsigned char _ccValue);
        virtual void loadCommands();
        virtual void requestState();
        virtual void sendNoteOn(uint8_t inNoteNumber,
                           uint8_t inVelocity,
                           uint8_t inChannel);
        virtual void sendNoteOff(uint8_t inNoteNumber,
                            uint8_t inVelocity,
                            uint8_t inChannel);
        virtual void sendProgramChange(uint8_t inProgramNumber,
                                  uint8_t inChannel);
        virtual void sendControlChange(uint8_t inControlNumber,
                                  uint8_t inControlValue,
                                  uint8_t inChannel);
        virtual void sendSysEx(unsigned inLength,
                          const byte* inArray,
                          bool inArrayContainsBoundaries = false);
};

#endif