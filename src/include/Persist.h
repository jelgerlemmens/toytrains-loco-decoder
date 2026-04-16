#include <EEPROM.h>

#define ROM_LOCATION_STATE 1022

#ifndef PERSISTH
#define PERSISTH

enum State { INITIAL, PROGRAMMED, ANALOG };

class Persist {
private:
    State state = INITIAL;  // default on first boot
public:
    Persist();
    ~Persist();
    int    getState();
    void   persistState(State state);
    char*  getStateName(State state);
};

#endif
