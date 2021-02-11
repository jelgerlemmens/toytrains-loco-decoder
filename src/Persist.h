
#include <EEPROM.h>

#define ROM_LOCATION_STATE 1022

#ifndef PERSISTH
#define PERSISTH
  enum State { INITIAL,PROGRAMMED,ANALOG };

class Persist {

  private:     
    State state = INITIAL; //standard State init on first boot    
  public:
    Persist::Persist();
    Persist::~Persist();
    int Persist::getState();
    void Persist::persistState(State state);
    char *Persist::getStateName(State state);    
};
#endif
