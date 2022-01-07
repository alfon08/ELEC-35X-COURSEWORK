//Group I // Authors Ryan Williams

#include "buffer.hpp"
#include "mbed.h"
#include "rtos/ThisThread.h"
#include "Sampling.hpp"
#include "net.hpp"


int iotLight;
float iotTemp;              //these will be sent to the IOT
float iotPress;                 
char iotdate_time[32];
Timer buzzT;                //timer for buzzer
bool AckPress = false;
DigitalOut redLED(TRAF_RED1_PIN);  // red led to hightlight buffer error
DigitalOut greenLED(TRAF_GRN1_PIN); //  green light to highlight buffer is healthy
microseconds SilenceT = 0s;
extern Thread t4;           // IOT thread for usage
Buzzer alarm;
extern int numberSamples;
extern void Flag_Set3();
int Lightarray[8] = {0};
float Temparray[8] = {0};
float Pressarray[8] {0};
int c =0;

void buffer::SpaceAllocate(char dt[32], int l, float T, float P){
    buffer* message = mail_box.try_alloc();
        if (message == NULL) {                  //error handling if failed to allocate space in buffer
            redLED = 1;                         // red light comes on
            error("No space in buffer\n");      //passes  error command
            greenLED = 0;                       // healthy light comes off
        return;             }
        int stringLength = strlen(dt);
        for(int i = 0; i <= (stringLength -1); i++){
        message->date_time[i] = dt[stringLength -1-i];
        iotdate_time[stringLength -1-i] = dt[stringLength -1-i];
        }
        date_time[stringLength] = '\0';
        iotdate_time[stringLength] = '\0';
        message-> ldr = l; //write passed values to message type buffer*
        message-> Temp = T;
        message-> Press = P;
        iotLight = l;
        iotTemp = T;
        iotPress = P;
        osStatus stat = mail_box.put(message);  //send message
            if (stat != osOK) {                 //if message fails error is recorded
                redLED = 1;                     //red light comes on
                error("buffering data failed\n"); //passes  error comman
                greenLED = 0;                   // helathy light off
                mail_box.free(message);         // remove message from mail_box
            return;
                    } 
            else{
                greenLED = 1;                   //healthy light on if buffer write successful}

        }

}


void buffer::checkvalues (int l, float T, float P){                 //Take values as parameter to check
    ldrVal = l;                                                   
    TempVal = T;                                                  //assign internally for ease of comparing
    PressVal = P;
    SilenceT = buzzT.elapsed_time();

    if ((AckPress == false) | (SilenceT > 60s)){
    if(ldrVal <= ldralarm_low) {    //set high and low using or condition to encapsulate boundary
        alarm.playTone("C", Buzzer::LOWER_OCTAVE);                  //play buzzer to notify threshold breached
        mainQueue.call(printf,"Light Level Low Warning!!\n");           //message added to queue to print when pointer points to it
        //    t5.flag_set(2);             //if threshold breached
    }

    if(ldrVal >= ldralarm_high){    //set high and low using or condition to encapsulate boundary
        alarm.playTone("C", Buzzer::HIGHER_OCTAVE);                  //play buzzer to notify threshold breached
        mainQueue.call(printf,"Light Level High Warning!!\n");           //message added to queue to print when pointer points to it
        //    t5.flag_set(2);             //if threshold breached
    }

    if(TempVal >= tempalarm_high | TempVal <= tempalarm_low){
        alarm.playTone("C", Buzzer::LOWER_OCTAVE);
        mainQueue.call(printf,"Temperature Level Warning!!\n");
        //    t5.flag_set(2);       //so if we want to test these, CERRorcount reduce to  3 and that way, if these get called, alarm will go of and reset the whole thing
    }

    if(PressVal >= pressalarm_high | PressVal <= pressalarm_low){
        alarm.playTone("C", Buzzer::LOWER_OCTAVE);
        mainQueue.call(printf,"Pressure Level Warning!!\n");
        //    t5.flag_set(2);
    }
    buzzT.reset();                                                  //reset buzzer so it doesns't ring continuously
    if (SilenceT > 60s){                                            //allow cancellation of alarm multiple times
        AckPress == false);}                                        //reset blue button status so can be pressed more than once to turn off alarm

    }
}
void buffer::updatearrays(int l, float T, float P){
    for(int i = 7; i >0; i --){
    Lightarray[i] = Lightarray[i -1];    
    Temparray[i] = Temparray[i -1];
    Pressarray[i] = Pressarray[i -1];}
    Lightarray[0] = l;
    Temparray[0] = T;
    Pressarray[0] = P;
    Flag_Set3();
}


void BuzzStop(){
    alarm.rest();                               //silence alarm
    AckPress = true;                            //bool for if switch has been pressed, in this case it has
    buzzT.start();                              //start timer and used above to check if 60s has passed
    mainQueue.call(printf,"Alarm Silenced\n");  //queue message that alarm has been silenced
}

void buzzstopISR(){
    wait_us(10000);             //let noise settle
    mainQueue.call(&BuzzStop);  //& is implied. calls function to cancel buzzer

void buffer::azureSetpoint(int x, char y, char z){
    if(y == 'L' && z == 'H')
    {
        ldralarm_high = x;
        mainQueue.call(printf, ("Light High Alarm Setpoint changed to: %d\n"), ldralarm_high);

    }

        if(y == 'L' && z == 'L')
    {
        ldralarm_low = x;
        mainQueue.call(printf, ("Light Low Alarm Setpoint changed to: %d\n"), ldralarm_low);
  
    }

}



