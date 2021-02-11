    #ifndef FUNCTIONCONTROLLERH
    #define FUNCTIONCONTROLLERH
    #include "PinController.h"        
    #include "ConfVarController.h"
    #include <NmraDcc.h>

    class FunctionController
    {
        private:
            void FunctionController::EvaluateFunctionGroupOne(int functionGroup, uint8_t functionState);      // fn 0 - 4
            void FunctionController::EvaluateFunctionGroupTwo(int functionGroup, uint8_t functionState);      // fn 5 - 8
            void FunctionController::EvaluateFunctionGroupThree(int functionGroup, uint8_t functionState);    // fn 9 - 12
            void FunctionController::EvaluateFunctionGroupFour(int functionGroup, uint8_t functionState);     // fn 13 - 20
            void FunctionController::EvaluateFunctionGroupFive(int functionGroup, uint8_t functionState);     // fn 21 - 28            
        public:
            FunctionController(ConfVarController * confVarController);
            ~FunctionController();    
            Function Functions[AVAILABLE_FUNCTIONS] = {}; 
            PinController * pinController;
            ConfVarController * confVarController;

            void BindFunctionsAndCVs();
            void FillFunctions();
            void ExecuteFunctions(int a[][2]);
            void PrintFunctionByNumber(Function * function);
            Function * GetFunctionByNumber(int functionNumber);
            void HandleFunction(int functionGroup, uint8_t functionState);
    };

    #endif
