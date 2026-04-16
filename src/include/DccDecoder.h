#include "FunctionController.h"
#include "Persist.h"

#ifndef DCCDECODERH
#define DCCDECODERH

FunctionController*  functionController;
ConfVarController*   confVarController;
Persist              persist;           // value, not pointer — no heap allocation needed
State                state;

void EvaluateAndExecuteState(State state);
void ExecuteInitialState();
void ExecuteProgrammedState();
void ExecuteAnalogState();
void softReset();

#endif
