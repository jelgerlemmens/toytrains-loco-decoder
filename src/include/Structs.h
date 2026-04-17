// A Configuration Value is the smallest combined unit of configuration.
// CV numbers fit in 8 bits for this decoder (max CV ID used: 63).
// CV values are always 8-bit per NMRA spec.

#ifndef STRUCTSH
#define STRUCTSH
#include <stdint.h>

enum PinType { DIGITAL, MOMENTARY_PULSE };

typedef struct CVPair
{
    uint8_t CV_ID;
    uint8_t Value;
};

typedef struct PinPair
{
    uint8_t Pin_ID;
    uint8_t State;
    PinType pinType;
};

//-----------------------------------------------------------------------------------------------------------------------------
/*
    A function can consist of multiple CV's and multiple pins, however in this case a single cv and single pin per function is used.
*/
typedef struct Function
{
    uint8_t  FunctionNumber;
    PinPair *pin_pair;
    CVPair   cv_pair;      // stored by value — was a dangling pointer to a local variable

    bool currentState;
};

#endif
