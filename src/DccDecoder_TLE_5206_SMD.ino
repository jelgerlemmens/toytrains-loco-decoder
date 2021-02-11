#include <NmraDcc.h>
#include "DccDecoder.h"
#include "UserConfig.h"
#include "AnalogRun.h"
//-----------------------------------------------------------------------------------------------------------------------------
NmraDcc Dcc;

/*
 * NOTE: Define DEBUG in UserConfig.h
 * Uses eeprom storage (!)
 */

void setup()
{  
  #ifdef DEBUG
    SERIALINTERFACE.begin(9600);
  #endif  
  //retrieve the persisted cab state, only on the init will the CV's be written to eeprom
  persist = new Persist();  
  state = persist->getState();
    #ifdef DEBUG
      SERIALINTERFACE.print("Found Decoder state: ");
      SERIALINTERFACE.println(persist->getStateName(state));
    #endif

    pinMode(ANALOG_OPS_PIN_1, INPUT);
    pinMode(ANALOG_OPS_PIN_2, INPUT);
    pinMode(DCC_INPUT_PIN, INPUT);
    pinMode(MOTOR_OUT_DIR_REV_PIN, OUTPUT);
    pinMode(MOTOR_OUT_DIR_FWD_PIN, OUTPUT);
    digitalWrite(MOTOR_OUT_DIR_FWD_PIN, HIGH);
    digitalWrite(MOTOR_OUT_DIR_REV_PIN, HIGH);   
    
  //evaluate and executes according to state
  EvaluateAndExecuteState(state);
  
}
  
//-----------------------------------------------------------------------------------------------------------------------------
void loop()
{
  //run on digital track
  if(state == PROGRAMMED){
    Dcc.process();    
  }
  //run on analog track
  if(state == ANALOG){
    AnalogRunControl::processLoop();
    
  }
}

//-----------------------------------------------------------------------------------------------------------------------------
void EvaluateAndExecuteState(State state){
  switch(state){
    case INITIAL : 
      ExecuteInitialState();
     break;
    case PROGRAMMED :
      ExecuteProgrammedState();
     break;
    case ANALOG :
      ExecuteAnalogState();
      break;
    default : 
      ExecuteProgrammedState();
      break;
  }
}

void ExecuteInitialState(){
  
  confVarController = new ConfVarController(Dcc);   
  functionController = new FunctionController(confVarController);
  
  //on INITIAL State the eeprom will have all registers set to 0
  //after eeprom init the CV's will be written to eeprom
  #ifdef DEBUG
    SERIALINTERFACE.println("Cab is in init state, starting base config...");
  #endif
  
  //init dcc
  Dcc.pin(0, DCC_INPUT_PIN, 0);
  Dcc.init(MAN_ID_DIY, 001, FLAGS_MY_ADDRESS_ONLY, 0);
  
  int decoderAddressInit = 3; //standard nmra dcc adress to initialize with
  
  //set all cv's for decoder to eeprom     
  confVarController->InitConfVars(decoderAddressInit);
  delete confVarController;
  
  //persist the current state to INITIAL
  persist->persistState(PROGRAMMED);
  
  #ifdef DEBUG
    SERIALINTERFACE.print("Init DCC cab address: ");
    SERIALINTERFACE.println(decoderAddressInit, DEC);
    SERIALINTERFACE.println("Cab init completed, CV's have now been stored to eeprom");
    SERIALINTERFACE.println("please reboot...");
    delay(2000); //wait for print statements to finish 
  #endif
  softReset(); //reset (rather jmp to 0)
}
//-----------------------------------------------------------------------------------------------------------------------------
void ExecuteProgrammedState(){
    //running on an analog track, coming from digital, switch state to analog and reset
    if(AnalogRunCheck::CheckIfAnalog()){
      #ifdef DEBUG  
        SERIALINTERFACE.println("Switching from Digital to Analog track ops"); 
      #endif        
      persist->persistState(ANALOG);
      delay(1000);//give time to persist state
      softReset();
    }    
    confVarController = new ConfVarController(Dcc);    
    functionController = new FunctionController(confVarController);
    #ifdef DEBUG    
      SERIALINTERFACE.print("Current DCC address: ");
      SERIALINTERFACE.println(Dcc.getCV(CV_MULTIFUNCTION_PRIMARY_ADDRESS), DEC);
    #endif
    Dcc.pin(0, DCC_INPUT_PIN, 0);  
    Dcc.init(MAN_ID_DIY, 001, FLAGS_MY_ADDRESS_ONLY, 0);
    functionController->BindFunctionsAndCVs();
    digitalWrite(MOTOR_OUT_DIR_FWD_PIN, HIGH);
    digitalWrite(MOTOR_OUT_DIR_REV_PIN, HIGH);  
    
}
//-----------------------------------------------------------------------------------------------------------------------------
void ExecuteAnalogState(){
    // for a 1/4 of a sec check if state is still analog 
    if(!AnalogRunCheck::CheckIfAnalog()){
      #ifdef DEBUG  
        SERIALINTERFACE.println("Switching from Analog to Digital track ops");
      #endif
      //if state isnt ANALOG, we're back on digital track. set state accordingly and resume
      persist->persistState(PROGRAMMED);
      delay(1000);//give time to persist state
      softReset();       
    } 
}

//-----------------------------------------------------------------------------------------------------------------------------
/*+
 * This function depends on the used Hbridge, currently the TLE5206-s by infineon is chosen. This Hbridge delivers a small formfactor with a 
 * high voltage/amperage capability  
 * 
 * notifyDccSpeed() Callback for a multifunction decoder speed command.
 *                   The received speed and direction are unpacked to separate values.
 *
 *  Inputs:
 *    Addr        - Active decoder address.
 *    AddrType    - DCC_ADDR_SHORT or DCC_ADDR_LONG.
 *    Speed       - Decoder speed. 0               = Emergency stop
 *                                 1               = Regular stop
 *                                 2 to SpeedSteps = Speed step 1 to max.
 *    Dir         - DCC_DIR_REV or DCC_DIR_FWD
 *    SpeedSteps  - Highest speed, SPEED_STEP_14   =  15
 *                                 SPEED_STEP_28   =  29
 *                                 SPEED_STEP_128  = 127
 *
 *  Returns:
 *    None
 */
void notifyDccSpeed(uint16_t Addr, DCC_ADDR_TYPE AddrType, uint8_t Speed, DCC_DIRECTION Dir, DCC_SPEED_STEPS SpeedSteps)
{
  //regular rolling stop
  if (Speed == 0 || Speed == 1)  
  { 
    digitalWrite(MOTOR_OUT_DIR_FWD_PIN, HIGH);
    digitalWrite(MOTOR_OUT_DIR_REV_PIN, HIGH);    
  }
  if (Speed > 1){
    //NOTE: The speed map below has been optimized for use with the TLE5206S (SMD) H-bridge see: http://www.farnell.com/datasheets/1651367.pdf (LOCKED ANTI PHASE CONTROL)
    //This means that: 
    //Range: 0   - 127 is CW  as PWM on pin: MOTOR_OUT_DIR_FWD_PIN with pin: MOTOR_OUT_DIR_REV_PIN to LOW
    //Range: 127 - 255 is CCW as PWM on pin: MOTOR_OUT_DIR_FWD_PIN with pin: MOTOR_OUT_DIR_REV_PIN to LOW
    if (Dir == DCC_DIR_FWD){  
      Speed = map(Speed,0,126,127,0);
      analogWrite(MOTOR_OUT_DIR_FWD_PIN, Speed);
      digitalWrite(MOTOR_OUT_DIR_REV_PIN, LOW);
    }else if (Dir == DCC_DIR_REV) {    
      Speed = map(Speed,0,126,127,255); 
      analogWrite(MOTOR_OUT_DIR_FWD_PIN, Speed);
      digitalWrite(MOTOR_OUT_DIR_REV_PIN, LOW);  
    }
  }
}
//-----------------------------------------------------------------------------------------------------------------------------
/*+
 *  notifyDccFunc() Callback for a multifunction decoder function command.
 *
 *  Inputs:
 *    Addr        - Active decoder address.
 *    AddrType    - DCC_ADDR_SHORT or DCC_ADDR_LONG.
 *    FuncGrp     - Function group. FN_0      - 14 speed step headlight function.
 *                                                                  Mask FN_BIT_00.
 *                                  FN_0_4    - Functions  0 to  4. Mask FN_BIT_00 - FN_BIT_04
 *                                  FN_5_8    - Functions  5 to  8. Mask FN_BIT_05 - FN_BIT_08
 *                                  FN_9_12   - Functions  9 to 12. Mask FN_BIT_09 - FN_BIT_12
 *                                  FN_13_20  - Functions 13 to 20. Mask FN_BIT_13 - FN_BIT_20 
 *                                  FN_21_28  - Functions 21 to 28. Mask FN_BIT_21 - FN_BIT_28
 *    FuncState   - Function state. Bitmask where active functions have a 1 at that bit.
 *                                  You must & FuncState with the appropriate
 *                                  FN_BIT_nn value to isolate a given bit.
 *  Returns:
 *    None
 */
void notifyDccFunc(uint16_t Addr, DCC_ADDR_TYPE AddrType, FN_GROUP FuncGrp, uint8_t FuncState)
{  
  functionController->HandleFunction(FuncGrp, FuncState);
}


//address asembly to jump to 0 does not reset registers
void softReset()
{
  asm volatile ("  jmp 0");
}



//-----------------------------------------------------------------------------------------------------------------------------
