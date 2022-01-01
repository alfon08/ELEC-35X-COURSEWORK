/*
 * Copyright (c) 2020 Arm Limited
 * SPDX-License-Identifier: Apache-2.0
 */
 
#include "mbed.h"   //this has the watchdog function in it
#include "uop_msb.h"
#include "rtos/ThisThread.h"
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
InterruptIn CIEbutton (BTN2_PIN);           //Critical error button interrupt
Sampling ldr(AN_LDR_PIN);                   // constructor for setting up Sampling
buffer mes(0, 0, 0.0, 0.0);                 // constructor for setting up the buffer
DigitalInOut redLed2 (TRAF_RED2_PIN);

Mail<buffer, 16> mail_box;                  //buffer holds 16 of type buffer

int SDwriteFreq = 16;                        //writes to SD card afer x number of samples
int numberSamples = 0;
volatile int CErrorcount ;                  //critical error count
const uint32_t RESET_TIME = 30000;          //30 sec countdown for watchdog

//matrix
extern char Lightarray[8];
extern char Temparray[8];
extern char Pressarray[8];
char y = 'L'; //matrix array set to light by default

microseconds sampRate = 100ms;              //sampling Rate
microseconds tmrUpdate = 0ms;               //time to compare 
Buzzer critAlarm;                           //to set of when critical error occurs
EventQueue mainQueue;

extern void BuzzStop();                     //call function to stop buzzer

//Timers
Ticker Samptick;                            //ISR for triggering sampling
Timer updatetmr;                            //Timer for triggering update 

//Threads
Thread t1(osPriorityAboveNormal);            //Sampling Thread 
Thread t2(osPriorityNormal);                 //Buffer Thread
Thread t3(osPriorityNormal);                 //SD card Thread
Thread t4(osPriorityNormal);                 //IOTHub Thread
Thread t5(osPriorityNormal);                 //critical error thread
Thread t6(osPriorityNormal);                 //matrix thread

//* all in one place for ease. look for *

void Flag_Set(){                            //Sets the flag to start sampling via ticker
     t1.flags_set(1);}


//Thread 1
void GetSample(){
        while(true){
            ThisThread::flags_wait_any(1); 
            ThisThread::flags_clear(1); 
            ldr.Sample();                   //function to take samples and accumulate values
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
                ldr.UpdateSample();
                //mainQueue.call(printf, "data recorded in buffer at %s", ldr.Samptime_date);
                //Record average samples in buffer
                mes.SpaceAllocate(ldr.Samptime_date, ldr.dataAVG.ldrEngAVG, ldr.dataAVG. TempAVG, ldr.dataAVG.PressAVG);//Allocate date, ldr, temp and pres in the buffer
                mes.checkvalues(ldr.dataAVG.ldrEngAVG, ldr.dataAVG. TempAVG, ldr.dataAVG.PressAVG); //Check if they are above or below limits->WARNING
                t4.flags_set(1);            // set flag for updating data on IOTHub
                
                i = ++i;                    // increement counter
                numberSamples++;
                    if(i == (SDwriteFreq)){ // if counter is equal to specified total stored samples write to sd
                        bool empt = mail_box.empty(); //blocks if buffer is empty
                            if(!empt){      // if not empty
                                t3.flags_set(1); //send signal to SD card thread
                                i = 0;      // reset counter integer
                                numberSamples = 0;
                                    }
                                          }
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
    while(true){
    if(y == 'L'){
        for(int i= 0; i<7; i++){
        start.BarLight(Lightarray[i], i);
        ThisThread::sleep_for(5000ms);
        }
    }
    if(y == 'T'){
        for(int i= 0; i<7; i++){
        start.BarLight(Temparray[i], i);
        ThisThread::sleep_for(5000ms);}}
    }
    if(y == 'P'){
        for(int i= 0; i<7; i++){
        start.BarLight(Pressarray[i], i);
        ThisThread::sleep_for(5000ms);}}
}

// * thread 5 *
void CritError(){   
    ThisThread::flags_wait_any(2); //block until this flag is set
    ThisThread::sleep_for(50ms);    //switch bounce to stabilise
    ThisThread::flags_clear(2);    //clear flag incase there were any due to switch bounce
    CriticalSectionLock::enable();
    CErrorcount++;
    CriticalSectionLock::disable();


    if (CErrorcount >=4) {
        CriticalSectionLock::enable();  //lock so no interrupts can interrupt it as ticker is an interrupt
        //maybe a mutex lock    -   incase the other threads get calledx    
        printf ("critical error has occured. system will restart in 30 seconds");
        alarmm.playTone("A", Buzzer::HIGHER_OCTAVE);      //if this doesn't work, probs just need to be initilised here to extern it as its in other file
        redLed2=0;
        //CErrorcount=0;
        Watchdog &watchdog = Watchdog::get_instance();  //gets instance of time referenced for the wtachdog
        watchdog.start(RESET_TIME);                     //starts watchdog timer for 30 sec. whole system resets after time is over
        //CIEbutton.waitforpress();                     //block by waiting for press
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


int main() {
    
    /*ISR button */
        CIEbutton.rise(critErrbtnISR);           //button A AT PG0
        //set traf light 2
        redLed2.output();
        redLed2.mode(PinMode::OpenDrainNoPull);
        redLed2 = 1;                             //as open drain, set to 1 keep off


    if (!connect()) return -1;                  // obtain network connection
    if (!setTime()) return -1;                  // obtain time and update RTC
        matrix_bar start;
        //start.BarLight(4093);
        SDCardSetup();                          // Sets up SD card
        btnA.rise(&Queue_Read);                 // ISR for blue button which reads SD card
        Bluebtn.rise(&BuzzStop);                // call function to stop buzzer by passing in address
        t1.start(GetSample);                    // sampling thread start
        t2.start(bufferSample);                 // buffer thread start
        t3.start(SDCardWrite);                  // sd card thread start
        t4.start(iotazure);                     // iothub thread start
        t5.start(CritError);                    // reset thread start
        t6.start(matrix_display);               // matrix thread star
        Samptick.attach(&Flag_Set, sampRate);   // ISR for control of sample rate
        updatetmr.start();                      // timer for buffer write
        
    while (true) {
       mainQueue.dispatch_forever();            // main Queue for serial message management
    }

};





