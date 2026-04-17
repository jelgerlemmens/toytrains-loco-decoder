#include <Arduino.h>
#include <NmraDcc.h>
#include "DccDecoder.h"
#include "UserConfig.h"
#include "AnalogRun.h"

//-----------------------------------------------------------------------------------------------------------------------------
NmraDcc Dcc;

// Forward declarations
void EvaluateAndExecuteState(State state);
void ExecuteInitialState();
void ExecuteProgrammedState();
void ExecuteAnalogState();
void softReset();

/*
 * NOTE: Define DEBUG in UserConfig.h
 * Uses eeprom storage (!)
 */

void initMotorPins()
{
    pinMode(MOTOR_OUT_DIR_FWD_PIN, OUTPUT);
    pinMode(MOTOR_OUT_DIR_REV_PIN, OUTPUT);
    digitalWrite(MOTOR_OUT_DIR_FWD_PIN, HIGH);
    digitalWrite(MOTOR_OUT_DIR_REV_PIN, HIGH);  // HIGH/HIGH = locked brake (TLE5206 locked anti-phase)
}

void setup()
{
  #ifdef DEBUG
    SERIALINTERFACE.begin(9600);
  #endif
  // retrieve the persisted cab state; only on INITIAL will CVs be written to eeprom
  state = persist.getState();
    #ifdef DEBUG
      SERIALINTERFACE.print("Found Decoder state: ");
      SERIALINTERFACE.println(persist.getStateName(state));
    #endif

    pinMode(ANALOG_OPS_PIN_1, INPUT);
    pinMode(ANALOG_OPS_PIN_2, INPUT);
    pinMode(DCC_INPUT_PIN, INPUT);
    initMotorPins();

  // evaluate and execute according to state
  EvaluateAndExecuteState(state);
}

//-----------------------------------------------------------------------------------------------------------------------------
void loop()
{
  // run on digital track
  if(state == PROGRAMMED){
    Dcc.process();
  }
  // run on analog track
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

  // on INITIAL state the eeprom will have all registers set to 0.
  // after eeprom init the CVs will be written to eeprom.
  #ifdef DEBUG
    SERIALINTERFACE.println("Cab is in init state, starting base config...");
  #endif

  // init dcc
  Dcc.pin(0, DCC_INPUT_PIN, 0);
  Dcc.init(MAN_ID_DIY, 001, FLAGS_MY_ADDRESS_ONLY, 0);

  int decoderAddressInit = 7; // NMRA DCC primary address (CV_MULTIFUNCTION_PRIMARY_ADDRESS)

  // write all CVs for decoder to eeprom
  confVarController->InitConfVars(decoderAddressInit);
  delete confVarController;

  // persist state as PROGRAMMED so next boot goes straight to DCC mode
  persist.persistState(PROGRAMMED);

  #ifdef DEBUG
    SERIALINTERFACE.print("Init DCC cab address: ");
    SERIALINTERFACE.println(decoderAddressInit, DEC);
    SERIALINTERFACE.println("Cab init completed, CV's have now been stored to eeprom");
    SERIALINTERFACE.println("please reboot...");
    delay(2000); // wait for print statements to finish
  #endif
  softReset();
}
//-----------------------------------------------------------------------------------------------------------------------------
void ExecuteProgrammedState(){
    // running on an analog track coming from digital — switch state and reset
    if(AnalogRunCheck::CheckIfAnalog()){
      #ifdef DEBUG
        SERIALINTERFACE.println("Switching from Digital to Analog track ops");
      #endif
      persist.persistState(ANALOG);
      delay(10); // EEPROM.put() takes ~3 ms; 10 ms is more than enough
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
    // check if DCC signal has returned
    if(!AnalogRunCheck::CheckIfAnalog()){
      #ifdef DEBUG
        SERIALINTERFACE.println("Switching from Analog to Digital track ops");
      #endif
      persist.persistState(PROGRAMMED);
      delay(10); // EEPROM.put() takes ~3 ms; 10 ms is more than enough
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

  // Speed 0 = emergency stop, Speed 1 = regular stop — HIGH/HIGH = locked brake
  if (Speed == 0 || Speed == 1)
  {
    digitalWrite(MOTOR_OUT_DIR_FWD_PIN, HIGH);
    digitalWrite(MOTOR_OUT_DIR_REV_PIN, HIGH);
    return;
  }

  // Locked anti-phase: FWD_PIN PWM (0–127 = FWD, 127–255 = REV), REV_PIN held LOW
  if (Dir == DCC_DIR_FWD) {
    Speed = map(Speed, 2, SpeedSteps, 127, 0);
    analogWrite(MOTOR_OUT_DIR_FWD_PIN, Speed);
    digitalWrite(MOTOR_OUT_DIR_REV_PIN, LOW);
  } else {
    Speed = map(Speed, 2, SpeedSteps, 127, 255);
    analogWrite(MOTOR_OUT_DIR_FWD_PIN, Speed);
    digitalWrite(MOTOR_OUT_DIR_REV_PIN, LOW);
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


// address assembly to jump to 0 — note: does not reset registers or peripherals
void softReset()
{
  asm volatile ("  jmp 0");
}


//-----------------------------------------------------------------------------------------------------------------------------
