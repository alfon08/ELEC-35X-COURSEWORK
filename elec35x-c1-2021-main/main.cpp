/*
 * Copyright (c) 2020 Arm Limited
 * SPDX-License-Identifier: Apache-2.0
 */
 
#include "mbed.h"
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

InterruptIn Bluebtn(USER_BUTTON);//used for printing out what is in the SD card
InterruptIn btnA(BTN1_PIN);//used for printing out what is in the SD card
Sampling ldr(AN_LDR_PIN);// constructor for setting up Sampling
buffer mes(0, 0, 0.0, 0.0); // constructor for setting up the buffer

Mail<buffer, 16> mail_box; //buffer holds 16 of type buffer
int SDwriteFreq = 4; //writes to SD card afer x number of samples
microseconds sampRate = 100ms; //sampling Rate

//Timers
Ticker Samptick; //ISR for triggering sampling
Timer updatetmr; //Timer for triggering update 

//Threads
Thread t1(osPriorityAboveNormal); //Sampling Thread 
Thread t2(osPriorityNormal); //Buffer Thread
Thread t3(osPriorityNormal); //SD card Thread
Thread t4(osPriorityNormal); //IOTHub Thread


microseconds tmrUpdate = 0ms; //time to compare 
EventQueue mainQueue;
extern void BuzzStop();


void Flag_Set(){ //Sets the flag to start sampling
     t1.flags_set(1);
                }



//Thread 1
void GetSample(){
        while(true){
            ThisThread::flags_wait_any(1); 
            ThisThread::flags_clear(1); 
            ldr.Sample(); //function to take samples and accumulate values
            tmrUpdate = updatetmr.elapsed_time(); //read value of timer
                if(tmrUpdate > 9000ms){//update every 9 seconds - just less than every 10s. 
                    updatetmr.reset(); // resets timer
                    t2.flags_set(1); // sets flag for buffer
                                    }
                     }
                }       

//Thread 2
void bufferSample(){
    char time_date[32]; 
    int i =0; //counter integer for buffer to sd write
            while(true){
                ThisThread::flags_wait_any(1); 
                ThisThread::flags_clear(1);
                //Take average of samples data and update previous readings
                ldr.UpdateSample();
                //mainQueue.call(printf, "data recorded in buffer at %s", ldr.Samptime_date);
                //Record average samples in buffer
                mes.SpaceAllocate(ldr.Samptime_date, ldr.dataAVG.ldrEngAVG, ldr.dataAVG. TempAVG, ldr.dataAVG.PressAVG);
                mes.checkvalues(ldr.dataAVG.ldrEngAVG, ldr.dataAVG. TempAVG, ldr.dataAVG.PressAVG);
                t4.flags_set(1); // set flag for updating data on IOTHub
                i = ++i; // increement counter
                    if(i == (SDwriteFreq)){ // if counter is equal to specified total stored samples write to sd
                        bool empt = mail_box.empty(); //blocks if buffer is empty
                            if(!empt){  // if not empty
                                t3.flags_set(1); //send signal to SD card thread
                                i = 0; // reset counter integer
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


int main() {
    

    //if (!connect()) return -1; // obtain network connection
    //if (!setTime()) return -1; // obtain time and update RTC
        matrix_bar start;
        start.BarLight(4093);
        SDCardSetup(); // Sets up SD card
        btnA.rise(&Queue_Read); //ISR for blue button which reads SD card
        Bluebtn.rise(&BuzzStop);
        t1.start(GetSample); // sampling thread start
        t2.start(bufferSample); // buffer thread start
        t3.start(SDCardWrite); // sd card thread start
        t4.start(iotazure); // iothub thread start
        Samptick.attach(&Flag_Set, sampRate); // ISR for control of sample rate
        updatetmr.start(); //timer for buffer write
        
    while (true) {
       mainQueue.dispatch_forever(); // main Queue for serial message management
    }

};





