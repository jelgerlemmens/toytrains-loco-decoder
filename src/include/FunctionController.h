    #ifndef FUNCTIONCONTROLLERH
    #define FUNCTIONCONTROLLERH
    #include "PinController.h"
    #include "ConfVarController.h"
    #include <NmraDcc.h>

    class FunctionController
    {
        private:
            void EvaluateFunctionGroupOne  (uint8_t functionState);   // fn  0 -  4
            void EvaluateFunctionGroupTwo  (uint8_t functionState);   // fn  5 -  8
            void EvaluateFunctionGroupThree(uint8_t functionState);   // fn  9 - 12
            void EvaluateFunctionGroupFour (uint8_t functionState);   // fn 13 - 20
            void EvaluateFunctionGroupFive (uint8_t functionState);   // fn 21 - 28
        public:
            FunctionController(ConfVarController* confVarController);
            ~FunctionController();
            Function          Functions[AVAILABLE_FUNCTIONS] = {};
            PinController*    pinController;
            ConfVarController* confVarController;

            void      BindFunctionsAndCVs();
            void      FillFunctions();
            void      ExecuteFunctions(int toBeExecuted[][2], int count);
            void      PrintFunctionByNumber(Function* function);
            Function* GetFunctionByNumber(uint8_t functionNumber);
            void      HandleFunction(int functionGroup, uint8_t functionState);
    };

    #endif
