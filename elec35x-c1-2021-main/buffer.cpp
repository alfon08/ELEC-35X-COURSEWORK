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
            if (stat != osOK) { //if message fails error is recorded
                redLED = 1; //red light comes on
                error("buffering data failed\n"); //passes  error comman
                greenLED = 0; // helathy light off
                mail_box.free(message); // remove message from mail_box
            return;
                    } 
            else{
                greenLED = 1; //healthy light on if buffer write successful}
}
}

void buffer::checkvalues (int l, float T, float P){                 //Take values as parameter to check
    ldralarm = l;                                                   
    Tempalarm = T;                                                  //assign internally for ease of comparing
    Pressalarm = P;
    SilenceT = buzzT.elapsed_time();
    if ((AckPress == false) | (SilenceT > 60s)){
    if((ldralarm >= ldralarm_high | ldralarm <= ldralarm_low) ){    //set high and low using or condition to encapsulate boundary

        alarm.playTone("C", Buzzer::LOWER_OCTAVE);                  //play buzzer to notify threshold breached
        mainQueue.call(printf,"Light Level Warning!!\n");           //message added to queue to print when pointer points to it
        //    t5.flag_set(2);             //if threshold breached
    }

    if(Tempalarm >= tempalarm_high | Tempalarm <= tempalarm_low){
        alarm.playTone("C", Buzzer::LOWER_OCTAVE);
        mainQueue.call(printf,"Temperature Level Warning!!\n");
        //    t5.flag_set(2);       //so if we want to test these, CERRorcount reduce to  3 and that way, if these get called, alarm will go of and reset the whole thing
    }

    if(Pressalarm >= pressalarm_high | Pressalarm <= pressalarm_low){
        alarm.playTone("C", Buzzer::LOWER_OCTAVE);
        mainQueue.call(printf,"Pressure Level Warning!!\n");
        //    t5.flag_set(2);
    }
    buzzT.reset();                                                  //reset buzzer so it doesns't ring continuously

    }
}

void BuzzStop(){
     alarm.rest();
     AckPress = true;   
     buzzT.start();
     mainQueue.call(printf,"Alarm Silenced\n");
}



