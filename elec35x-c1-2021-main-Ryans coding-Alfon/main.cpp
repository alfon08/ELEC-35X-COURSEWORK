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
#include "azure_c_shared_utility/xlogging.h"
using namespace uop_msb;

LCD_16X2_DISPLAY disp;
InterruptIn btn(USER_BUTTON);
//extern void SDCardRead();
Mail<buffer, 16> mail_box;
Ticker Samptick; //ISR for triggering sample every 10ms
Ticker SDtick;
Sampling ldr(AN_LDR_PIN);// Constructor for setting up Sampling
buffer mes(0, 0, 0.0, 0.0);
Thread t1(osPriorityAboveNormal);
Thread t2(osPriorityNormal);
Thread t3(osPriorityNormal);
Thread t4(osPriorityNormal);
Timer updatetmr; //Timer for triggering update 
microseconds tmrUpdate = 0ms; //time to compare 
int i =0; //counter integer for buffer to sd write
//extern void azureDemo();
//void iotazure();

int SDwriteFreq = 4; //ever x samples write to sd 
microseconds sampRate = 100ms; 

EventQueue mainQueue;


void Flag_Set(){
     t1.flags_set(1);
                }



//Thread 1
void GetSample(){
        while(true){
            ThisThread::flags_wait_any(1); 
            ThisThread::flags_clear(1); 
            ldr.Sample();
            tmrUpdate = updatetmr.elapsed_time();
                if(tmrUpdate > 9000ms){//update every 9 seconds - just less than every 10s. 
                    updatetmr.reset();
                    t2.flags_set(1);
                                    }
                     }
                }       

//Thread 2
void bufferSample(){
    char time_date[32];
            while(true){
                ThisThread::flags_wait_any(1); 
                ThisThread::flags_clear(1);
                ldr.UpdateSample();
                //write to buffer non-blocking ()
                // time_t seconds = time(NULL);
                // strftime(time_date, 32, "%x: %I:%M:%S %p\n", localtime(&seconds));
                mainQueue.call(printf, "data recorded in buffer at %s", ldr.Samptime_date);
                mes.SpaceAllocate(ldr.Samptime_date, ldr.dataAVG.ldrEngAVG, ldr.dataAVG.TempAVG, ldr.dataAVG.PressAVG);
                t4.flags_set(1);
                i = ++i;
                    if(i == (SDwriteFreq)){ //Write to sd card 
                        bool empt = mail_box.empty(); //blocks if buffer is empty
                            if(!empt){ 
                                t3.flags_set(1); //send signal to SD card thread
                                i = 0; // reset counter integer
                                    }
                                          }
                     }                

                }

//Thread 4
void iotazure(){
    while(true){
                    iothubrecord();
    }
}


int main() {
    

    if (!connect()) return -1;

    if (!setTime()) return -1;

        SDCardSetup();
        btn.rise(&Queue_Read);
        t1.start(GetSample);
        t2.start(bufferSample);
        t3.start(SDCardWrite);
        t4.start(iotazure);
        Samptick.attach(&Flag_Set, sampRate);
        updatetmr.start();
        
    while (true) {
       mainQueue.dispatch_forever();
    }

    //return 0;
};





