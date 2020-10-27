//--------------------config--------------------
#include <Arduino.h>
#ifndef USERCONFH
#define USERCONFH

        //NANO
        #if defined ARDUINO_AVR_NANO
            #define DCC_INPUT_PIN               2  //interrupt 0 pin on NANO / UNO
            #define AVAILABLE_FUNCTIONS         9  //amount of Digital pins 3,4,5,6,10,11,12,13 are available for functions on NANO: amount of available pins = amount of available functions
            #define AVAILABLE_FUNCTION_PINS     3,4,5,6,7,8,11,12,13 //list of available pins
            #define MOTOR_OUT_DIR_FWD_PIN       9 //pin for h-bridge motor control forward  Hbrdige used: TLE5206, both pins need PWM capability
            #define MOTOR_OUT_DIR_REV_PIN       10 //pin for h-bridge motor control reverse  Hbrdige used: TLE5206, both pins need PWM capability
            #define ANALOG_OPS_PIN_1            A0 //pin used for reading voltage from track, If loco is on analog track, these pins are used to convert power to engine    
            #define ANALOG_OPS_PIN_2            A1 //pin used for reading voltage from track, If loco is on analog track, these pins are used to convert power to engine
        #endif

        //debugging
       //#define DEBUG
        #ifdef DEBUG
          #define SERIALINTERFACE Serial 
        #endif
#endif

//--------------------config--------------------
