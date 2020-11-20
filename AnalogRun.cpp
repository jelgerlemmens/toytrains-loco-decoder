#include "AnalogRun.h"

namespace AnalogRunCheck
{
     //call after pin init when loc is prgrammed state
     bool CheckIfAnalog(){
        int millisPeriod = 100; // 1/6 of a sec
        int negCount = 0;
        int posCount = 0;      
        unsigned long time_now = millis();
        
        //evaluate state of DCC pin for 1/4 of a second
        while(millis() < time_now + millisPeriod){
          int readval = digitalRead(DCC_INPUT_PIN);
          if(!readval){            
            negCount++;            
          }else{
            posCount++;
          }
        }
        #ifdef DEBUG   
            SERIALINTERFACE.println("NEGCOUNT: ");
            SERIALINTERFACE.print(negCount,DEC);
            SERIALINTERFACE.println("POSCOUNT: ");
            SERIALINTERFACE.print(posCount,DEC);
        #endif
        //check if the pin state fluctuates 
        if(negCount > 0 && posCount > 0){                          
            return false; // is not analog running on a digital signal
        }
        else if(negCount == 0 && posCount > 0){
          return CheckIfPower();
        }
        else if(negCount > 0 && posCount == 0){
          return CheckIfPower();
        }        
        else if(negCount == 0 && posCount == 0){
            return false; // no power whatsoever, this means we are debugging, or at least there is no active track power
        }
    }

    bool CheckIfPower(){
      int valueOpsPin1 = analogRead(ANALOG_OPS_PIN_1);
      int valueOpsPin2 = analogRead(ANALOG_OPS_PIN_2);
      if(valueOpsPin1  > 0 || valueOpsPin2 > 0){
        return true;
      }else{
        return false;
      }
      
    }
}

namespace AnalogRunControl
{
    void processLoop(){

    //read pin vals, very often preferably 
    int valueOpsPin1 = analogRead(ANALOG_OPS_PIN_1);
    int valueOpsPin2 = analogRead(ANALOG_OPS_PIN_2);

//      #ifdef DEBUG   
//        SERIALINTERFACE.println("AnalogOps pin 1: ");
//        SERIALINTERFACE.print(valueOpsPin1,DEC);
//        SERIALINTERFACE.println("AnalogOps pin 2: ");
//        SERIALINTERFACE.print(valueOpsPin2,DEC);
//      #endif


    // if both pins are 0, loc is in full stop 
    if(valueOpsPin1 == 0 && valueOpsPin2 == 0){
      digitalWrite(MOTOR_OUT_DIR_FWD_PIN, HIGH);
      digitalWrite(MOTOR_OUT_DIR_REV_PIN, HIGH);      
    }

    // if ANALOG_OPS_PIN_1 is > 0 forward motion
    if(valueOpsPin1 > 0 && valueOpsPin2 == 0){
      //Set tle5206 to forward
      int aSpeed = map(valueOpsPin1,0,300,255,10);
      digitalWrite(MOTOR_OUT_DIR_REV_PIN, HIGH);       
      analogWrite(MOTOR_OUT_DIR_FWD_PIN, aSpeed);       
    }
    
    // if ANALOG_OPS_PIN_2 is > 0 backward motion 
    if(valueOpsPin1 == 0 && valueOpsPin2 > 0){
      //Set tle5206 to reverse
      int aSpeed = map(valueOpsPin2,0,300,255,10);
      digitalWrite(MOTOR_OUT_DIR_FWD_PIN, HIGH);      
      analogWrite(MOTOR_OUT_DIR_REV_PIN, aSpeed);   
      
    }
          
        
    

    }
}
