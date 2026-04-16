#include "UserConfig.h"
#include "Structs.h"

#ifndef PINCONTROLLERH
#define PINCONTROLLERH

class PinController
{
public:
    PinPair  Pins[AVAILABLE_FUNCTIONS] = {};
    PinController();
    ~PinController();
    void     InitPins();
    void     FillPins();
    void     SetPins();
    void     PrintPins();
    PinPair* GetPinPair(uint8_t Pin_ID);
    void     SetPinState(PinPair* pinPair);
};

#endif
