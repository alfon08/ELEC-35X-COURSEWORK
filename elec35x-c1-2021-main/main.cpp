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
#include "Windows.h"

InterruptIn Bluebtn(USER_BUTTON);           //used for printing out what is in the SD card
InterruptIn btnA(BTN1_PIN);                 //used for printing out what is in the SD card
Sampling ldr(AN_LDR_PIN);                   // constructor for setting up Sampling
buffer mes(0, 0, 0.0, 0.0);                 // constructor for setting up the buffer

Mail<buffer, 16> mail_box;                  //buffer holds 16 of type buffer
int SDwriteFreq = 4;                        //writes to SD card afer x number of samples
microseconds sampRate = 100ms;              //sampling Rate

//Timers
Ticker Samptick;                            //ISR for triggering sampling
Timer updatetmr;                            //Timer for triggering update 

//Threads
Thread t1(osPriorityAboveNormal);            //Sampling Thread 
Thread t2(osPriorityNormal);                 //Buffer Thread
Thread t3(osPriorityNormal);                 //SD card Thread
Thread t4(osPriorityNormal);                 //IOTHub Thread

//* all in one place for ease. look for *
Thread t5(osPriorityNormal);                //critical error thread 
DigitalInOut redLed2 (TRAF_RED2_PIN);
volatile int CErrorcount ;                  //critical error count
const uint32_t RESET_TIME = 30000;          //30 sec countdown for watchdog


microseconds tmrUpdate = 0ms;               //time to compare 
EventQueue mainQueue;
extern void BuzzStop();


void Flag_Set(){                            //Sets the flag to start sampling
     t1.flags_set(1);
                }


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
                mes.SpaceAllocate(ldr.Samptime_date, ldr.dataAVG.ldrEngAVG, ldr.dataAVG. TempAVG, ldr.dataAVG.PressAVG);
                mes.checkvalues(ldr.dataAVG.ldrEngAVG, ldr.dataAVG. TempAVG, ldr.dataAVG.PressAVG);
                t4.flags_set(1);            // set flag for updating data on IOTHub
                i = ++i;                    // increement counter
                    if(i == (SDwriteFreq)){ // if counter is equal to specified total stored samples write to sd
                        bool empt = mail_box.empty(); //blocks if buffer is empty
                            if(!empt){      // if not empty
                                t3.flags_set(1); //send signal to SD card thread
                                i = 0;      // reset counter integer
                                    }
                                          }
                     }                

                }

//Thread 4
void iotazure(){ 
    while(true){
                    //iothubrecord(); // iothub function

    }
}

void matrix_display(char y){
    if(y == 'L'){
        start.BarLight(ldr.dataAVG.ldrEngAVG, 6);
        mainQueue.call(printf,"Matrix Display - Light\n");
    }
    if(y == 'T'){
        start.BarTemp(ldr.dataAVG.TempAVG, 6);
        mainQueue.call(printf,"Matrix Display - Temperature\n");
    }
    if(y == 'P'){
        mainQueue.call(printf,"Matrix Display - Pressure\n");}}

// * thread 5 *
void CritError(){
    this thread::flags_wait_any(2); //block until this flag is set
    this thread_sleep_for(50ms);    //switch bounce to stabilise
    this thread::flags_clear(2);    //clear flag incase there were any due to switch bounce
    CErrorcount++;

    if (CErrorcount >=4) {
        printf ("critical error has occured. system will restart in 30 seconds");
        alarm.playTone("C", Buzzer::LOWER_OCTAVE);      //if this doesn't work, probs just need to be initilised here to extern it as its in other file
        redLed2=0;
        //CErrorcount=0;
        Watchdog &watchdog = Watchdog::get_instance();
        watchdog.start(RESET_TIME);              //starts watchdog timer for 30 sec. whole system resets after time is over
        CIEbutton.waitforpress();                //block by waiting for press
        //sleep();
    }

}
//**
void critErrbtnISR (){          //interrupt service routine for the switch at PG_1
    CIEbutton.rise(NULL)
    t5.flag_set(2);             //trying to keep the interrupt short as possible and 
                                //let function do the work
}


int main() {
    
    /*ISR button */
        CIEbutton.rise(critErrbtnISR)           //button A AT PG0
        //set traf light 2
        redLed2.output();
        redLed2.mode(PinMode::OpenDrainNoPull);
        redLed2 = 1;             //as open drain, set to 0 keep off


    if (!connect()) return -1; // obtain network connection
    if (!setTime()) return -1; // obtain time and update RTC
        //matrix_bar start;
        //start.BarLight(4093);
        SDCardSetup();          // Sets up SD card
        btnA.rise(&Queue_Read); //ISR for blue button which reads SD card
        Bluebtn.rise(&BuzzStop);
        t1.start(GetSample);    // sampling thread start
        t2.start(bufferSample); // buffer thread start
        t3.start(SDCardWrite);  // sd card thread start
        t4.start(iotazure);     // iothub thread start
        t5.start(CritError)     // reset thread 
        Samptick.attach(&Flag_Set, sampRate);   // ISR for control of sample rate
        updatetmr.start();      //timer for buffer write
        
    while (true) {
       mainQueue.dispatch_forever();            // main Queue for serial message management
    }

};





