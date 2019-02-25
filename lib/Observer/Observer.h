#ifndef DLOBSERVER
#define DLOBSERVER

class DLObserver {
    public:
        virtual void clearControlChanges(unsigned char _ccNum, unsigned char _ccValue);
};

#endif