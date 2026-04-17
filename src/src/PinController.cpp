#include "PinController.h"

PinController::PinController(/* args */)
{
}
    
PinController::~PinController()
{
}


void PinController::InitPins(){

    this->FillPins();
    this->SetPins();

    #ifdef DEBUG
        this->PrintPins();
    #endif

}

void PinController::FillPins(){

    int AvailableFunctionPins[AVAILABLE_FUNCTIONS] = {AVAILABLE_FUNCTION_PINS};
    for(int i = 0; i < AVAILABLE_FUNCTIONS; i++){
        Pins[i].Pin_ID  = AvailableFunctionPins[i];
        if(AvailableFunctionPins[i] == HORN_PIN){
            Pins[i].pinType = MOMENTARY_PULSE;
            Pins[i].State   = LOW; // idle LOW — BSS138 FET OFF, DFPlayer IO_1 pulled HIGH by internal pull-up
        } else {
            Pins[i].pinType = DIGITAL;
            Pins[i].State   = LOW;
        }
    }
}

void PinController::SetPins()
{
    for(int i = 0; i <  AVAILABLE_FUNCTIONS; i++)
    {
        pinMode(Pins[i].Pin_ID, OUTPUT);
        digitalWrite(Pins[i].Pin_ID, Pins[i].State); // uses initial state — HIGH for horn, LOW for others
    }
}

PinPair* PinController::GetPinPair(uint8_t Pin_ID)
{
    for(int i = 0; i < AVAILABLE_FUNCTIONS; i++){
        if(Pins[i].Pin_ID == Pin_ID){ return &Pins[i]; }
    }
    return NULL;  // pin not found — was missing, causing undefined behaviour
}

void PinController::SetPinState(PinPair * pinPair){
    if(pinPair->Pin_ID > 0 &&  pinPair->State >= 0){
      #ifdef DEBUG
        SERIALINTERFACE.println("---------{PinController::SetPinState}---------");
        SERIALINTERFACE.println("Setting pin state to: ");
        SERIALINTERFACE.print("PIN_ID: ");
        SERIALINTERFACE.println(pinPair->Pin_ID, DEC);
        SERIALINTERFACE.print("PIN_STATE: ");
        SERIALINTERFACE.println(pinPair->State, DEC);
        SERIALINTERFACE.println("---------{PinController::SetPinState}---------");
      #endif
        if(pinPair->pinType == MOMENTARY_PULSE){
            if(pinPair->State == HIGH){
                // activation: tone through FET to piezo — two short blasts like a steam whistle
                tone(pinPair->Pin_ID, 3000, 300);
                delay(400);
                tone(pinPair->Pin_ID, 3000, 300);
                delay(150);
                noTone(pinPair->Pin_ID);
                pinPair->State = LOW;
            }
            // deactivation: do nothing
        } else {
            digitalWrite(pinPair->Pin_ID, pinPair->State);
        }
    }
}

void PinController::PrintPins(){
    #ifdef DEBUG
      SERIALINTERFACE.println("---------{PinController::PrintPins}---------");
      SERIALINTERFACE.println("Printing pins...");
      for(int i = 0; i <  AVAILABLE_FUNCTIONS; i++)
      {
          SERIALINTERFACE.print("PIN_ID: ");
          SERIALINTERFACE.println(Pins[i].Pin_ID, DEC);
          SERIALINTERFACE.print("PIN_STATE: ");
          SERIALINTERFACE.println(Pins[i].State, DEC);        
      }
      SERIALINTERFACE.println("---------{PinController::PrintPins}---------");
    #endif
}
      
    
