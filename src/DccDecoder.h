#include "FunctionController.h"    
#include "Persist.h"
#ifndef DCCDECODERH
#define DCCDECODERH
    FunctionController * functionController;
    ConfVarController * confVarController;
    Persist * persist;
    State state;
    void EvaluateAndExecuteFunction(FN_GROUP functionGroup, uint8_t functionState);
    int EvaluateFunctionAndSetState(FN_GROUP functionGroup, uint8_t functionState);
    void ExecuteFunction(int functionNumber, uint8_t functionState);   
    void EvaluateAndExecuteState(State state);
    void ExecuteInitialState();
    void ExecuteProgrammedState();
    void ExecuteAnalogState();
    void softReset();
#endif
