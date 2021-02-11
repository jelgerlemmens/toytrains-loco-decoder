//A Configuration Value is the smallest combined unit of configuration.
//16 bits for CV numbers
// 8 bits for cv values

#ifndef STRUCTSH
#define STRUCTSH


typedef struct CVPair
{
    int  CV_ID;
    int  Value;
};


typedef struct PinPair
{
    int  Pin_ID;
    int   State;
};
//-----------------------------------------------------------------------------------------------------------------------------
/*
    A function can consist of multiple CV's and multiple pins, however in this case a single cv and single pin per function is used.    
*/
typedef struct Function
{
    int FunctionNumber;
    PinPair *pin_pair;
    CVPair *cv_pair;

    bool currentState;
};

#endif
