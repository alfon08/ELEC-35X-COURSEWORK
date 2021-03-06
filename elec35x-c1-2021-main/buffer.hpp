//Group I // Authors Ryan Williams
#ifndef buffer_hpp
#define buffer_hpp
#include "Sampling.hpp"
#include "mbed.h"
#include "uop_msb.h"
#include "mbed.h"
#include "rtos/ThisThread.h"
#include "Sampling.hpp"


class buffer { 
    private:
        //Private Variables
        int ldrVal;
        float TempVal;
        float PressVal;             //internal variables to hold data
        bool LHAlarmSet = false;
        bool LLAlarmSet = false;

    public:
        //Public Variables
        volatile int ldr;                    // integer for ldr buffered sample - volatile because they are shared
        volatile float Temp;                 // float for temp buffered sample
        volatile float Press;                // float for press buffered sample
        char date_time[32];         // string for date and time
        int samplesInBuffer = 0;

        //Public Functions
        void SpaceAllocate(char dt[32], int l, float T, float P); // function to write to buffer
        void checkvalues (int l, float T, float P);               //function to check values
        void BuzzStop();                                          //function to cancel buzzer
        void updatearrays(int l, float T, float P);               //function to update arrays for arrays
        void azureSetpoint(int x, char y, char z);                //fuction to set/change alarm threshold from iot
        //void azureSetpointT(int x, char y, char z); //is this needed?
        //void azureSetpointP(int x, char y, char z); //is this needed?

    //Constructor
         buffer(char dt[32], int l, float t, float p) {
                    int stringLength = strlen(dt);                        // finds length of string
                        for(int i = 0; i <= (stringLength -1); i++){          // iterates through string
                            date_time[i] = dt[stringLength -1-i];             // write to new char time and date
                        }
                    date_time[stringLength] = '\0'; // sets 0 to all unfilled array data
                    ldr = l;                // sets passed in value for l to ldr
                    Temp = t;               // sets passed in value for t to temp
                    Press = p;              // sets passed in value for p to press
                                                }
        //Public Variables
        int ldralarm_high = 50000;              // internal constraint of values defined for safe usage
        int ldralarm_low = 30000;               // outside these thresholds are deemed dangerous
        float tempalarm_low = 15.00;            // changing these values will change threshold boundary
        float tempalarm_high = 22.00;
        float pressalarm_low = 500.00;
        float pressalarm_high = 1500.00;
};

//External:
extern Mail<buffer, 16> mail_box; // brings in mailbox which can be seen at main
extern EventQueue mainQueue;      // queues onto main queue for sequencial order
extern Watchdog &watchdog;
extern Thread t4;           // IOT thread for usage
//extern int numberSamples;  
extern int SDwriteFreq;

#endif
