#include "FunctionController.h"
//-----------------------------------------------------------------------------------------------------------------------------
    FunctionController::FunctionController(ConfVarController * confVarController)
    {
        this->pinController = new PinController();
        this->confVarController = confVarController;
    }
//-----------------------------------------------------------------------------------------------------------------------------    
    FunctionController::~FunctionController()
    {
        delete pinController;
    }
    
//-----------------------------------------------------------------------------------------------------------------------------
    void FunctionController::BindFunctionsAndCVs(){
        
        //initialize pins for board
        pinController->InitPins();
        
        //Fill AvailableFunctions
        this->FillFunctions();            

    }
//-----------------------------------------------------------------------------------------------------------------------------
    void FunctionController::FillFunctions()
    {

         int AvailableFunctionPins[AVAILABLE_FUNCTIONS] = {AVAILABLE_FUNCTION_PINS};

        for(size_t i = 0; i < AVAILABLE_FUNCTIONS; i++)
        {
            Functions[i].FunctionNumber = i;
            Functions[i].pin_pair = pinController->GetPinPair(AvailableFunctionPins[i]);            
            //loc functions start at CV 35
            int cvid = i+34;            
            CVPair cvpair = confVarController->GetCVPair(cvid);               
            Functions[i].cv_pair = &cvpair;            
            Functions[i].currentState = false;
            #ifdef DEBUG              
              PrintFunctionByNumber(&Functions[i]);
            #endif
            
        }
    }
//-----------------------------------------------------------------------------------------------------------------------------
    Function * FunctionController::GetFunctionByNumber(int functionNumber)
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
            this->EvaluateFunctionGroupOne(functionGroup, functionState);
            break;
            
        case FN_5_8:    
           this->EvaluateFunctionGroupTwo(functionGroup, functionState);
            break;
            
        case FN_9_12:
           this->EvaluateFunctionGroupThree(functionGroup, functionState);
            break;

        case FN_13_20: 
           this->EvaluateFunctionGroupFour(functionGroup, functionState);
            break;
            
        case FN_21_28:
           this->EvaluateFunctionGroupFour(functionGroup, functionState);
            break;	
        }
    } 

//-----------------------------------------------------------------------------------------------------------------------------
void FunctionController::ExecuteFunctions(int toBeExecuted[][2]){
  
    for(int i = 0 ; i < 5; i++){
      Function * function = GetFunctionByNumber(toBeExecuted[i][1]);
      function->cv_pair->Value = toBeExecuted[i][0];
      function->pin_pair->State = toBeExecuted[i][0];
      this->pinController->SetPinState(function->pin_pair);
      this->confVarController->StoreCvPairValue(function->cv_pair);
    }
  
}


//-----------------------------------------------------------------------------------------------------------------------------
void FunctionController::EvaluateFunctionGroupOne(int functionGroup, uint8_t functionState){

  // create map of which functions should be turned on / off in this group.
  //functions 0-4
  int toBeExecuted[5][2] = {
      //to be executed 0/1, functionNumber
      {false,0}, //executed: no, Function 0, 
      {false,1}, //executed: no, Function 1, 
      {false,2}, //executed: no, Function 2, 
      {false,3}, //executed: no, Function 3, 
      {false,4}, //executed: no, Function 4,     
    };

    //set functions which should be executed:     
    toBeExecuted[0][0] = (functionState & FN_BIT_00) >> 4;
    toBeExecuted[1][0] = (functionState & FN_BIT_01);
    toBeExecuted[2][0] = (functionState & FN_BIT_02) >> 1;
    toBeExecuted[3][0] = (functionState & FN_BIT_03) >> 2;
    toBeExecuted[4][0] = (functionState & FN_BIT_04) >> 3;
    
     #ifdef DEBUG
     SERIALINTERFACE.println("Printing execution map");        
     for(int i = 0 ; i < 5; i++){        
        SERIALINTERFACE.print("Execute 0/1: ");
        SERIALINTERFACE.println(toBeExecuted[i][0],DEC);                      
        SERIALINTERFACE.print("Function number: ");
        SERIALINTERFACE.println(toBeExecuted[i][1],DEC);                      
        SERIALINTERFACE.println("");
     }
    #endif

    this->ExecuteFunctions(toBeExecuted);
    
    return;
}
//-----------------------------------------------------------------------------------------------------------------------------
void FunctionController::EvaluateFunctionGroupTwo(int functionGroup, uint8_t functionState){
    //176

    // create map of which functions should be turned on / off in this group.
    //functions 5 - 8
    int toBeExecuted[5][2] = {
      //to be executed 0/1, functionNumber
      {false,5}, //executed: no, Function 0, 
      {false,6}, //executed: no, Function 1, 
      {false,7}, //executed: no, Function 2, 
      {false,8}, //executed: no, Function 3,     
    };

    //set functions which should be executed:     
    toBeExecuted[5][0] = (functionState & FN_BIT_05);
    toBeExecuted[6][0] = (functionState & FN_BIT_06) >> 1;
    toBeExecuted[7][0] = (functionState & FN_BIT_07) >> 2;
    toBeExecuted[8][0] = (functionState & FN_BIT_08) >> 3;
    
     #ifdef DEBUG
     SERIALINTERFACE.println("Printing execution map");        
     for(int i = 0 ; i < 5; i++){        
        SERIALINTERFACE.print("Execute 0/1: ");
        SERIALINTERFACE.println(toBeExecuted[i][0],DEC);                      
        SERIALINTERFACE.print("Function number: ");
        SERIALINTERFACE.println(toBeExecuted[i][1],DEC);                      
        SERIALINTERFACE.println("");
     }
    #endif

    this->ExecuteFunctions(toBeExecuted);
    
    return;
}
//-----------------------------------------------------------------------------------------------------------------------------
void FunctionController::EvaluateFunctionGroupThree(int functionGroup, uint8_t functionState){

   // create map of which functions should be turned on / off in this group.
    //functions 5 - 8
    int toBeExecuted[5][2] = {
      //to be executed 0/1, functionNumber
      {false,9}, //executed: no, Function 0, 
      {false,10}, //executed: no, Function 1, 
      {false,11}, //executed: no, Function 2, 
      {false,12}, //executed: no, Function 3,     
    };

    //set functions which should be executed:     
    toBeExecuted[9][0] = (functionState & FN_BIT_09);
    toBeExecuted[10][0] = (functionState & FN_BIT_10) >> 1;
    toBeExecuted[11][0] = (functionState & FN_BIT_11) >> 2;
    toBeExecuted[12][0] = (functionState & FN_BIT_12) >> 3;
    
     #ifdef DEBUG
     SERIALINTERFACE.println("Printing execution map");        
     for(int i = 0 ; i < 5; i++){        
        SERIALINTERFACE.print("Execute 0/1: ");
        SERIALINTERFACE.println(toBeExecuted[i][0],DEC);                      
        SERIALINTERFACE.print("Function number: ");
        SERIALINTERFACE.println(toBeExecuted[i][1],DEC);                      
        SERIALINTERFACE.println("");
     }
    #endif

    this->ExecuteFunctions(toBeExecuted);
    
    return;
}
//-----------------------------------------------------------------------------------------------------------------------------
void FunctionController::EvaluateFunctionGroupFour(int functionGroup, uint8_t functionState){  
    //todo
}
//-----------------------------------------------------------------------------------------------------------------------------
void FunctionController::EvaluateFunctionGroupFive(int functionGroup, uint8_t functionState){
   //todo
}
//-----------------------------------------------------------------------------------------------------------------------------
void FunctionController::PrintFunctionByNumber(Function * function){
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
      SERIALINTERFACE.println(function->cv_pair->CV_ID, DEC);
      SERIALINTERFACE.print("Linked cv_pair.Value: ");
      SERIALINTERFACE.println(function->cv_pair->Value, DEC);
      SERIALINTERFACE.println("---------{PrintFunctionByNumber}---------");
    #endif
}
