#include "Persist.h"

Persist::Persist(){
  
}
Persist::~Persist(){
  
}

int Persist::getState(){
  EEPROM.get(ROM_LOCATION_STATE, state );  
  return state;
}
void Persist::persistState(State state){
  EEPROM.put(ROM_LOCATION_STATE, state); 
}

char *Persist::getStateName(State state) 
{
   switch (state) 
   {
      case INITIAL: return "INITIAL";
      break;
      case PROGRAMMED: return "PROGRAMMED";
      break;
      case ANALOG: return "ANALOG";
      break;
   }
}
