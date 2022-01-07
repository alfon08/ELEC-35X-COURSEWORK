/*
 * Copyright (c) 2020 Arm Limited
 * SPDX-License-Identifier: Apache-2.0
 */
 
#include "mbed.h"   //this has the watchdog function in it
#include "uop_msb.h"
#include "rtos/ThisThread.h"
#include <cstdio>
#include <cstring>
#include <string.h>
#include "Sampling.hpp"
#include <ctype.h>
#include "buffer.hpp"
#include "SDBlockDevice.h"
#include <iostream>
#include "sd.hpp"
#include "net.hpp"
#include "matrix.hpp"

InterruptIn Bluebtn(USER_BUTTON);           //used for printing out what is in the SD card
InterruptIn btnA(BTN1_PIN);                 //used for printing out what is in the SD card
Sampling Samp(AN_LDR_PIN);                   // constructor for setting up Sampling
buffer mes(0, 0, 0.0, 0.0);                 // constructor for setting up the buffer
LCD_16X2_DISPLAY disp;
Mail<buffer, 16> mail_box;                  //buffer holds 16 of type buffer
int SDwriteFreq = 16;                        //writes to SD card afer x number of samples
microseconds sampRate = 100ms;              //sampling Rate
int numberSamples = 0;
//Timers
Ticker Samptick;                            //ISR for triggering sampling
Timer updatetmr;                            //Timer for triggering update 


//mutex
Mutex mutex;

//Threads
Thread t1(osPriorityAboveNormal);            //Sampling Thread 
Thread t2(osPriorityNormal);                 //Buffer Thread
Thread t3(osPriorityNormal);                 //SD card Thread
Thread t4(osPriorityNormal);                 //IOTHub Thread

//* all in one place for ease. look for *
Thread t5(osPriorityNormal);                //critical error thread
Thread t6(osPriorityNormal);
DigitalInOut redLed2 (TRAF_RED2_PIN);
volatile int CErrorcount ;                  //critical error count
const uint32_t RESET_TIME = 30000;          //30 sec countdown for watchdog
InterruptIn CIEbutton (BTN2_PIN);
Buzzer alarmm;
extern int Lightarray[8];
extern float Temparray[8];
extern float Pressarray[8];
extern bool LHAlarmset;
extern bool LLAlarmset;
extern int iotLight;
bool SPUpdate = false;
char y = 'L'; //matrix array set to light by default
microseconds tmrUpdate = 0ms;               //time to compare 
EventQueue mainQueue;
extern void BuzzStop();



void Flag_Set(){                            //Sets the flag to start sampling
     t1.flags_set(1);}

void Flag_Set2(){                            //
     t3.flags_set(1);}

void Flag_Set3(){                            //
     t6.flags_set(1);}

//Thread 1
void GetSample(){
        while(true){
            ThisThread::flags_wait_any(1); 
            ThisThread::flags_clear(1); 
            Samp.Sample();                   //function to take samples and accumulate values
            tmrUpdate = updatetmr.elapsed_time(); //read value of timer
                if(tmrUpdate > 9000ms){     //update every 9 seconds - just less than every 10s. 
                    updatetmr.reset();      // resets timer
                    t2.flags_set(1);        // sets flag for buffer
                                    }
                     }
                }       

//Thread 2
void bufferSample(){
    char time_date[32]; 
    int i =0;                               //counter integer for buffer to sd write
            while(true){
                ThisThread::flags_wait_any(1); 
                ThisThread::flags_clear(1);
                //Take average of samples data and update previous readings
                Samp.UpdateSample();
                //mainQueue.call(printf, "data recorded in buffer at %s", ldr.Samptime_date);
                //Record average samples in buffer
                mes.SpaceAllocate(Samp.Samptime_date, Samp.dataAVG.ldrEngAVG, Samp.dataAVG.TempAVG, Samp.dataAVG.PressAVG);//Allocate date, ldr, temp and pres in the buffer
                mes.checkvalues(Samp.dataAVG.ldrEngAVG, Samp.dataAVG.TempAVG, Samp.dataAVG.PressAVG); //Check if they are above or below limits->WARNING
                mes.updatearrays(Samp.dataAVG.ldrEngAVG, Samp.dataAVG.TempAVG, Samp.dataAVG.PressAVG);
                t4.flags_set(1);            // set flag for updating data on IOTHub
                i = ++i;                    // increement counter
                numberSamples++;
                    if(i == (SDwriteFreq)){ // if counter is equal to specified total stored samples write to sd
                        bool empt = mail_box.empty(); //blocks if buffer is empty
                            if(!empt){      // if not empty
                                t3.flags_set(1); //send signal to SD card thread
                                i = 0;      // reset counter integer
                                numberSamples = 0;  //reset number of samples
                                    }
                                          }
                     }                

                }

void AzureSP_check(int x, char y, char z){
    if(LHAlarmset == true){
        mes.azureSetpoint(x, y, z);
        LHAlarmset = false;

    }
    if(LLAlarmset == true){
        mes.azureSetpoint(x, y, z);
        LLAlarmset = false;
    }


}

//Thread 4
void iotazure(){ 
    while(true){
                    iothubrecord(); // iothub function

    }
}

//Thread 6
void matrix_display() {
    matrix_bar start;
    start.clearMatrix();
    disp.cls();
    disp.locate(1, 0);
    while(true){
    disp.locate(1, 0);
    disp.printf("Display=%c\n",y);
        if(y == 'L'){
            for(int i= 0; i<=7; i++){
            start.BarLight(Lightarray[i], i);
        }}
        if(y == 'T'){
            for(int i= 0; i<=7; i++){
            start.BarTemp(Temparray[i], i);
            }}
        if(y == 'P'){
            for(int i= 0; i<=7; i++){
            start.BarPres(Pressarray[i], i);
        }}}
}

// * thread 5 *
void CritError(){   
    ThisThread::flags_wait_any(2); //block until this flag is set
    ThisThread::sleep_for(50ms);    //switch bounce to stabilise
    ThisThread::flags_clear(2);    //clear flag incase there were any due to switch bounce
    CErrorcount++;

    if (CErrorcount >=4) {
        CriticalSectionLock::enable();  //lock so no interrupts can interrupt it as ticker is an interrupt
        printf ("critical error has occured. system will restart in 30 seconds");
        alarmm.playTone("C", Buzzer::LOWER_OCTAVE);      //if this doesn't work, probs just need to be initilised here to extern it as its in other file
        redLed2=0;                                       //turn on red led
        //CErrorcount=0;
        Watchdog &watchdog = Watchdog::get_instance();
        watchdog.start(RESET_TIME);              //starts watchdog timer for 30 sec. whole system resets after time is over
        //CIEbutton.waitforpress();              //block by waiting for press
        CriticalSectionLock::disable();
        //sleep();
    }

}
//**
void critErrbtnISR (){          //interrupt service routine for the switch at PG_1
    CIEbutton.rise(NULL);
    t5.flags_set(2);            //trying to keep the interrupt short as possible and 
                                //let function do the work
}

                }

int main() {
    
    /*ISR button */
        CIEbutton.rise(critErrbtnISR);           //button A AT PG0
        //set traf light 2
        redLed2.output();
        redLed2.mode(PinMode::OpenDrainNoPull);
        redLed2 = 1;           //as open drain, set to 0 keep off

    if (!connect()) return -1; // obtain network connection
    if (!setTime()) return -1; // obtain time and update RTC
        matrix_bar start;
        SDCardSetup();          // Sets up SD card
        btnA.rise(&Queue_Read); // ISR for blue button which reads SD card
        Bluebtn.rise(&buzzstopISR);// ISR to cancel buzzer for 1 minute
        t1.start(GetSample);    // sampling thread start
        //t1.join();
        t2.start(bufferSample); // buffer thread start
        //t2.join();        //waiting for each thread to finish 
        t3.start(SDCardWrite);  // sd card thread start
        //t3.join();
        t4.start(iotazure);     // iothub thread start
        t5.start(CritError);    // reset thread  - probably dont eed this
        t6.start(matrix_display);
        Samptick.attach(&Flag_Set, sampRate);   // ISR for control of sample rate
        updatetmr.start();      //timer for buffer write
        
    while (true) {
       mainQueue.dispatch_forever();            // main Queue for serial message management
    }

};





