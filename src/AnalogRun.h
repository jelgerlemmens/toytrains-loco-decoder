#include "UserConfig.h"

#ifndef ANALOGRUNCHECKH
#define ANALOGRUNCHECKH

namespace AnalogRunCheck
{
    bool CheckIfAnalog(); 
    bool CheckIfPower();       
}

namespace AnalogRunControl
{
  void processLoop();
  
}
#endif
