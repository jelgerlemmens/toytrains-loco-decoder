#include "UserConfig.h"
#include "Structs.h"

#define STARTPINNANO 2

#ifndef PINCONTROLLERH
#define PINCONTROLLERH

    class PinController
    {
    private:

    public:
        PinPair Pins[AVAILABLE_FUNCTIONS] = {};
        PinController(/* args */);
        ~PinController();
        void InitPins();
        void FillPins();
        void SetPins();
        void PrintPins();
        PinPair *GetPinPair(int Pin_ID);
        void PinController::SetPinState(PinPair * pinPair);
    };
    
#endif
