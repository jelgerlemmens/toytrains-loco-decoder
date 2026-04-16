#include "FunctionController.h"
//-----------------------------------------------------------------------------------------------------------------------------
FunctionController::FunctionController(ConfVarController* confVarController)
{
    this->pinController    = new PinController();
    this->confVarController = confVarController;
}
//-----------------------------------------------------------------------------------------------------------------------------
FunctionController::~FunctionController()
{
    delete pinController;
}

//-----------------------------------------------------------------------------------------------------------------------------
void FunctionController::BindFunctionsAndCVs(){
    pinController->InitPins();
    this->FillFunctions();
}
//-----------------------------------------------------------------------------------------------------------------------------
void FunctionController::FillFunctions()
{
    int AvailableFunctionPins[AVAILABLE_FUNCTIONS] = {AVAILABLE_FUNCTION_PINS};

    for(size_t i = 0; i < AVAILABLE_FUNCTIONS; i++)
    {
        Functions[i].FunctionNumber = i;
        Functions[i].pin_pair       = pinController->GetPinPair(AvailableFunctionPins[i]);
        // loc functions start at CV 35 (index i+34 for i=0..8 → CVs 34-42)
        uint8_t cvid            = i + 34;
        Functions[i].cv_pair    = confVarController->GetCVPair(cvid); // stored by value, no dangling pointer
        Functions[i].currentState = false;
        #ifdef DEBUG
            PrintFunctionByNumber(&Functions[i]);
        #endif
    }
}
//-----------------------------------------------------------------------------------------------------------------------------
Function* FunctionController::GetFunctionByNumber(uint8_t functionNumber)
{
    for(size_t i = 0; i < AVAILABLE_FUNCTIONS; i++)
    {
        if(functionNumber == Functions[i].FunctionNumber){
            return &Functions[i];
        }
    }
    return NULL;
}
//-----------------------------------------------------------------------------------------------------------------------------
void FunctionController::HandleFunction(int functionGroup, uint8_t functionState){

    #ifdef DEBUG
        SERIALINTERFACE.println("---------{EvaluateFunctionAndSetState}---------");
        SERIALINTERFACE.print("FN_GROUP  ");
        SERIALINTERFACE.println(functionGroup, DEC);
        SERIALINTERFACE.print("FuncState BIN ");
        SERIALINTERFACE.println(functionState, BIN);
        SERIALINTERFACE.print("FuncState DEC ");
        SERIALINTERFACE.println(functionState, DEC);
    #endif

    switch(functionGroup)
    {
        case FN_0_4:
            this->EvaluateFunctionGroupOne(functionState);
            break;
        case FN_5_8:
            this->EvaluateFunctionGroupTwo(functionState);
            break;
        case FN_9_12:
            this->EvaluateFunctionGroupThree(functionState);
            break;
        case FN_13_20:
            this->EvaluateFunctionGroupFour(functionState);
            break;
        case FN_21_28:
            this->EvaluateFunctionGroupFive(functionState);  // was incorrectly calling GroupFour
            break;
    }
}

//-----------------------------------------------------------------------------------------------------------------------------
// count: number of valid rows in toBeExecuted, so the loop doesn't overrun into uninitialised stack memory
void FunctionController::ExecuteFunctions(int toBeExecuted[][2], int count){
    for(int i = 0; i < count; i++){
        Function* function = GetFunctionByNumber(toBeExecuted[i][1]);
        if(function == NULL) continue;  // function number has no assigned pin — skip safely
        bool newState = toBeExecuted[i][0] != 0;
        if(function->currentState == newState) continue; // no change — skip pin write and EEPROM
        function->currentState       = newState;
        function->cv_pair.Value      = toBeExecuted[i][0];
        function->pin_pair->State    = toBeExecuted[i][0];
        this->pinController->SetPinState(function->pin_pair);
        this->confVarController->StoreCvPairValue(&function->cv_pair);
    }
}

//-----------------------------------------------------------------------------------------------------------------------------
void FunctionController::EvaluateFunctionGroupOne(uint8_t functionState){
    // functions 0 - 4
    int toBeExecuted[5][2] = {
        {0,0}, {0,1}, {0,2}, {0,3}, {0,4}
    };
    // FN_BIT_00 = 0x10 (bit 4), F1-F4 occupy bits 0-3
    toBeExecuted[0][0] = (functionState & FN_BIT_00) >> 4;
    toBeExecuted[1][0] = (functionState & FN_BIT_01);
    toBeExecuted[2][0] = (functionState & FN_BIT_02) >> 1;
    toBeExecuted[3][0] = (functionState & FN_BIT_03) >> 2;
    toBeExecuted[4][0] = (functionState & FN_BIT_04) >> 3;

    #ifdef DEBUG
        SERIALINTERFACE.println("Printing execution map");
        for(int i = 0; i < 5; i++){
            SERIALINTERFACE.print("Execute 0/1: ");
            SERIALINTERFACE.println(toBeExecuted[i][0], DEC);
            SERIALINTERFACE.print("Function number: ");
            SERIALINTERFACE.println(toBeExecuted[i][1], DEC);
            SERIALINTERFACE.println("");
        }
    #endif

    this->ExecuteFunctions(toBeExecuted, 5);
}
//-----------------------------------------------------------------------------------------------------------------------------
void FunctionController::EvaluateFunctionGroupTwo(uint8_t functionState){
    // functions 5 - 8
    int toBeExecuted[4][2] = {
        {0,5}, {0,6}, {0,7}, {0,8}
    };
    // indices 0-3 match the 4-row array — was incorrectly using indices 5-8 (out of bounds)
    toBeExecuted[0][0] = (functionState & FN_BIT_05);
    toBeExecuted[1][0] = (functionState & FN_BIT_06) >> 1;
    toBeExecuted[2][0] = (functionState & FN_BIT_07) >> 2;
    toBeExecuted[3][0] = (functionState & FN_BIT_08) >> 3;

    #ifdef DEBUG
        SERIALINTERFACE.println("Printing execution map");
        for(int i = 0; i < 4; i++){
            SERIALINTERFACE.print("Execute 0/1: ");
            SERIALINTERFACE.println(toBeExecuted[i][0], DEC);
            SERIALINTERFACE.print("Function number: ");
            SERIALINTERFACE.println(toBeExecuted[i][1], DEC);
            SERIALINTERFACE.println("");
        }
    #endif

    this->ExecuteFunctions(toBeExecuted, 4);
}
//-----------------------------------------------------------------------------------------------------------------------------
void FunctionController::EvaluateFunctionGroupThree(uint8_t functionState){
    // functions 9 - 12
    int toBeExecuted[4][2] = {
        {0,9}, {0,10}, {0,11}, {0,12}
    };
    // indices 0-3 match the 4-row array — was incorrectly using indices 9-12 (out of bounds)
    toBeExecuted[0][0] = (functionState & FN_BIT_09);
    toBeExecuted[1][0] = (functionState & FN_BIT_10) >> 1;
    toBeExecuted[2][0] = (functionState & FN_BIT_11) >> 2;
    toBeExecuted[3][0] = (functionState & FN_BIT_12) >> 3;

    #ifdef DEBUG
        SERIALINTERFACE.println("Printing execution map");
        for(int i = 0; i < 4; i++){
            SERIALINTERFACE.print("Execute 0/1: ");
            SERIALINTERFACE.println(toBeExecuted[i][0], DEC);
            SERIALINTERFACE.print("Function number: ");
            SERIALINTERFACE.println(toBeExecuted[i][1], DEC);
            SERIALINTERFACE.println("");
        }
    #endif

    this->ExecuteFunctions(toBeExecuted, 4);
}
//-----------------------------------------------------------------------------------------------------------------------------
void FunctionController::EvaluateFunctionGroupFour(uint8_t functionState){
    // functions 13 - 20 (full byte, bits 0-7)
    // Note: functions > 8 have no physical pin on this decoder — ExecuteFunctions will skip them via NULL check
    int toBeExecuted[8][2] = {
        {0,13},{0,14},{0,15},{0,16},{0,17},{0,18},{0,19},{0,20}
    };
    toBeExecuted[0][0] = (functionState & FN_BIT_13);
    toBeExecuted[1][0] = (functionState & FN_BIT_14) >> 1;
    toBeExecuted[2][0] = (functionState & FN_BIT_15) >> 2;
    toBeExecuted[3][0] = (functionState & FN_BIT_16) >> 3;
    toBeExecuted[4][0] = (functionState & FN_BIT_17) >> 4;
    toBeExecuted[5][0] = (functionState & FN_BIT_18) >> 5;
    toBeExecuted[6][0] = (functionState & FN_BIT_19) >> 6;
    toBeExecuted[7][0] = (functionState & FN_BIT_20) >> 7;

    #ifdef DEBUG
        SERIALINTERFACE.println("Printing execution map");
        for(int i = 0; i < 8; i++){
            SERIALINTERFACE.print("Execute 0/1: ");
            SERIALINTERFACE.println(toBeExecuted[i][0], DEC);
            SERIALINTERFACE.print("Function number: ");
            SERIALINTERFACE.println(toBeExecuted[i][1], DEC);
            SERIALINTERFACE.println("");
        }
    #endif

    this->ExecuteFunctions(toBeExecuted, 8);
}
//-----------------------------------------------------------------------------------------------------------------------------
void FunctionController::EvaluateFunctionGroupFive(uint8_t functionState){
    // functions 21 - 28 (full byte, bits 0-7)
    // Note: functions > 8 have no physical pin on this decoder — ExecuteFunctions will skip them via NULL check
    int toBeExecuted[8][2] = {
        {0,21},{0,22},{0,23},{0,24},{0,25},{0,26},{0,27},{0,28}
    };
    toBeExecuted[0][0] = (functionState & FN_BIT_21);
    toBeExecuted[1][0] = (functionState & FN_BIT_22) >> 1;
    toBeExecuted[2][0] = (functionState & FN_BIT_23) >> 2;
    toBeExecuted[3][0] = (functionState & FN_BIT_24) >> 3;
    toBeExecuted[4][0] = (functionState & FN_BIT_25) >> 4;
    toBeExecuted[5][0] = (functionState & FN_BIT_26) >> 5;
    toBeExecuted[6][0] = (functionState & FN_BIT_27) >> 6;
    toBeExecuted[7][0] = (functionState & FN_BIT_28) >> 7;

    #ifdef DEBUG
        SERIALINTERFACE.println("Printing execution map");
        for(int i = 0; i < 8; i++){
            SERIALINTERFACE.print("Execute 0/1: ");
            SERIALINTERFACE.println(toBeExecuted[i][0], DEC);
            SERIALINTERFACE.print("Function number: ");
            SERIALINTERFACE.println(toBeExecuted[i][1], DEC);
            SERIALINTERFACE.println("");
        }
    #endif

    this->ExecuteFunctions(toBeExecuted, 8);
}
//-----------------------------------------------------------------------------------------------------------------------------
void FunctionController::PrintFunctionByNumber(Function* function){
    #ifdef DEBUG
        SERIALINTERFACE.println("---------{PrintFunctionByNumber}---------");
        SERIALINTERFACE.print("Printing function with number:  ");
        SERIALINTERFACE.println(function->FunctionNumber, DEC);
        SERIALINTERFACE.print("and current state ");
        SERIALINTERFACE.println(function->currentState, DEC);
        SERIALINTERFACE.print("Linked pinPair.Pin_ID: ");
        SERIALINTERFACE.println(function->pin_pair->Pin_ID, DEC);
        SERIALINTERFACE.print("Linked pinPair.state: ");
        SERIALINTERFACE.println(function->pin_pair->State, DEC);
        SERIALINTERFACE.print("Linked cv_pair.CV_ID: ");
        SERIALINTERFACE.println(function->cv_pair.CV_ID, DEC);   // cv_pair is now a value, not pointer
        SERIALINTERFACE.print("Linked cv_pair.Value: ");
        SERIALINTERFACE.println(function->cv_pair.Value, DEC);
        SERIALINTERFACE.println("---------{PrintFunctionByNumber}---------");
    #endif
}
