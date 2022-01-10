/*
 * Copyright (c) 2020 Arm Limited
 * SPDX-License-Identifier: Apache-2.0
 */
#include "main.hpp"

//System paramters:
Mail<buffer, 16> mail_box;                  //buffer holds 16 of type buffer
int SDwriteFreq = 9;                       //writes to SD card afer x number of samples
microseconds sampRate = 100ms;              //sampling Rate

//interrupts
InterruptIn Bluebtn(USER_BUTTON);           //used for acknowlegde alarm
InterruptIn btnA(BTN1_PIN);                 //used for printing out what is in the SD card


//Class ints
Sampling Samp(AN_LDR_PIN);                  // constructor for setting up Sampling
buffer mes(0, 0, 0.0, 0.0);                 // constructor for setting up the buffer
Buzzer alarmm;
DigitalInOut redLed2 (TRAF_RED2_PIN);
LCD_16X2_DISPLAY disp;
bool SPUpdate = false;
char y = NULL;                               //matrix array set to light by default
microseconds tmrUpdate = 0ms;               //time to compare 
EventQueue mainQueue;
matrix_bar start;

//Timers and clock related variables
Ticker Samptick;                            //ISR for triggering sampling
Timer updatetmr;                            //Timer for triggering update 

const uint32_t RESET_TIME = 30000;          //30 sec countdown for watchdog
int numberSamples = 0;

//Threads
Thread t1(osPriorityAboveNormal);           //Sampling Thread 
Thread t2(osPriorityNormal);                //Buffer Thread
Thread t3(osPriorityNormal);                //SD card Thread
Thread t4(osPriorityNormal);                //IOTHub Thread
Thread t6(osPriorityNormal);                //matric thread

//                                    THREADS                                                  

//Thread 1
void GetSample(){
        while(true){
            ThisThread::flags_wait_any(1);            //Triggered by SampTick via flagset function
            ThisThread::flags_clear(1);               // clear flags
            Samp.Sample();                            //function to take samples and accumulate values
            tmrUpdate = updatetmr.elapsed_time();     //read value of timer
                if(tmrUpdate > 9000ms){               //update every 9 seconds - just less than every 10s. 
                    updatetmr.reset();                // resets timer
                    t2.flags_set(1);                  // sets flag for buffer
                }
        }
}       

//Thread 2
void bufferSample(){
    int i =0;                                            //counter integer for buffer to sd write
            while(true){
                ThisThread::flags_wait_any(1);          //Triggered by signal from getsample thread
                ThisThread::flags_clear(1);             // clear flags
                Samp.UpdateSample();                    //Take average of samples data and update previous readings
                Watchdog::get_instance().kick();        //kick watchdog
                                                        //check for space and allocate:
                mes.SpaceAllocate(Samp.Samptime_date, Samp.dataAVG.ldrEngAVG, Samp.dataAVG.TempAVG, Samp.dataAVG.PressAVG);
                                                        //Check alarm setpoints against readings
                mes.checkvalues(Samp.dataAVG.ldrEngAVG, Samp.dataAVG.TempAVG, Samp.dataAVG.PressAVG); 
                                                        //update matrix arrays buffer
                mes.updatearrays(Samp.dataAVG.ldrEngAVG, Samp.dataAVG.TempAVG, Samp.dataAVG.PressAVG);
                t4.flags_set(1);                        // set flag for updating data on IOTHub
                i = ++i;                                // increement counter
                numberSamples++;                        // numberSamples counter used for counting buffer
                    if(i == (SDwriteFreq)){             // if counter is equal to specified total stored samples write to sd
                        bool empt = mail_box.empty();   //blocks if buffer is empty
                            if(!empt){                  // if not empty semephore
                                t3.flags_set(1);        //send signal to SD card thread
                                i = 0;                  // reset counter integer
                                numberSamples = 0;      //reset number of samples
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
void matrix_display() { //function for displaying quantised values on matrix
    matrix_bar start; // init matrix - create chilc class called start
    start.clearMatrix(); // clear matrix
        while(true){                
                if(y == 'L'){
                    for(int i= 0; i<=7; i++){
                        start.BarLight(Lightarray[i], i);
                    }
                }

                if(y == 'T'){
                    for(int i= 0; i<=7; i++){
                        start.BarTemp(Temparray[i], i);
                    }
                }

                if(y == 'P'){
                    for(int i= 0; i<=7; i++){
                        start.BarPres(Pressarray[i], i);
                    }
                }
        }
}

//                                  MAIN                                                

int main() {
    
        //set traf light 2
        redLed2.output(); 
        redLed2.mode(PinMode::OpenDrainNoPull);
        redLed2 = 1;                                    //as open drain, set to 0 keep off
        if (!connect()) return -1;                      // obtain network connection
        if (!setTime()) return -1;                      // obtain time and update RTC
        SDCardSetup();                                  // Sets up SD card
        btnA.rise(&Queue_Read);                         // ISR for button A which reads SD card
        Bluebtn.rise(&BuzzStop);                        // ISR to cancel buzzer for 1 minute
        t1.start(GetSample);                            // sampling thread start
        t2.start(bufferSample);                         // buffer thread start
        t3.start(SDCardWrite);                          // sd card thread start
        t4.start(iotazure);                             // iothub thread start
        t6.start(matrix_display);                       // matric thread start
        Watchdog &watchdog = Watchdog::get_instance();  // get instance of watchdog
        watchdog.start(RESET_TIME);                     // start watchdog timer
        Samptick.attach(&Flag_Set, sampRate);           // ISR for control of sample rate
        updatetmr.start();                              // timer for buffer write
        
            while (true) {
                mainQueue.dispatch_forever();            // main Queue for serial message management
            }
};

//                            Flagset and other connecting functions

void Flag_Set(){                            // Sets the flag to start sampling
     t1.flags_set(1);}

void Flag_Set2(){                           // function to start SD card write
     t3.flags_set(1);}

void AzureSP_check(int x, char y, char z){  //function recieves alarm setpoint and L/T/P and H/L
        mes.azureSetpoint(x, y, z); //updates values within child class
}



