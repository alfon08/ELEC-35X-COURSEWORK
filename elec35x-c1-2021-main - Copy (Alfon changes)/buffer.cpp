//Group I // Authors Ryan Williams

#include "buffer.hpp"
#include "mbed.h"
#include "rtos/ThisThread.h"
#include "Sampling.hpp"
#include "net.hpp"
#include "matrix.hpp"

//Queues - "A message can be a integer or pointer value  to a certain type T that is sent to a thread or interrupt service routine."
Queue<uint32_t, 10> queue;

int iotLight;
float iotTemp;
float iotPress;
char iotdate_time[32];

Timer buzzT;
bool AckPress = false;
DigitalOut redLED(TRAF_RED1_PIN);  // red led to hightlight buffer error
DigitalOut greenLED(TRAF_GRN1_PIN); //  green light to highlight buffer is healthy
microseconds SilenceT = 0s;
extern Thread t4;
Buzzer alarm;

void buffer::SpaceAllocate(char dt[32], int l, float T, float P){
    buffer* message = mail_box.try_alloc();
        if (message == NULL) { //error handling if failed to allocate space in buffer
            redLED = 1; // red light comes on
            error("No space in buffer\n"); //passes  error command
            greenLED = 0; // healthy light comes off
        return;             }
        int stringLength = strlen(dt);
        for(int i = 0; i <= (stringLength -1); i++){
        message->date_time[i] = dt[stringLength -1-i];
        iotdate_time[stringLength -1-i] = dt[stringLength -1-i];
        }
        date_time[stringLength] = '\0';
        iotdate_time[stringLength] = '\0';
        message-> ldr = l; //write passed values to message type buffer*
        //Write to matrix queue as 32-bit integer (same size as pointer)
        bool sent = queue.try_put((uint32_t*)ldr); //Non-blocking
        message-> Temp = T;
        message-> Press = P;
        iotLight = l;
        iotTemp = T;
        iotPress = P;
        osStatus stat = mail_box.put(message);  //send message
            if (stat != osOK) { //if message fails error is recorde
                redLED = 1; //red light comes on
                error("buffering data failed\n"); //passes  error comman
                greenLED = 0; // helathy light off
                mail_box.free(message); // remove message from mail_box
            return;
                    } else{
        greenLED = 1; //healthy light on if buffer write successful}
}
}

void buffer::checkvalues (int l, float T, float P){
    ldralarm = l;
    Tempalarm = T;
    Pressalarm = P;
    SilenceT = buzzT.elapsed_time();
    if ((AckPress == false) | (SilenceT > 60s)){
    if((ldralarm >= ldralarm_high | ldralarm <= ldralarm_low) ){

        alarm.playTone("C", Buzzer::LOWER_OCTAVE);
        mainQueue.call(printf,"Light Level Warning!!\n");

    }

    if(Tempalarm >= tempalarm_high | Tempalarm <= tempalarm_low){
        alarm.playTone("C", Buzzer::LOWER_OCTAVE);
        mainQueue.call(printf,"Temperature Level Warning!!\n");

    }

    if(Pressalarm >= pressalarm_high | Pressalarm <= pressalarm_low){
        alarm.playTone("C", Buzzer::LOWER_OCTAVE);
        mainQueue.call(printf,"Pressure Level Warning!!\n");
    }
    buzzT.reset();

    }
}

void BuzzStop(){
     alarm.rest();
     AckPress = true;
     buzzT.start();
     mainQueue.call(printf,"Alarm Silenced\n");
}

void matrix_bar::plotTemp()
{
    for (int row = 0; row < 8; row++){

        uint32_t** rx;
        bool success = queue.try_get_for(5s, rx);
        uint32_t value = (uint32_t)rx;
        printf("Sample %i: %i", row, value);
        matrix_bar::BarLight(value, row);

    }
}

