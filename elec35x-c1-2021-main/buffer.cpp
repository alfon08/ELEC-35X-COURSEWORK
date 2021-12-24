//Group I // Authors Ryan Williams

#include "buffer.hpp"
#include "mbed.h"
#include "rtos/ThisThread.h"
#include "Sampling.hpp"

//#include "uop_msb.h"

DigitalOut redLED(TRAF_RED1_PIN);
DigitalOut greenLED(TRAF_GRN1_PIN);



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
        }
        date_time[stringLength] = '\0';
        message-> ldr = l; //write passed values to message type buffer*
        message-> Temp = T;
        message-> Press = P;
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




