//Group I // Authors Ryan Williams

#include "buffer.hpp"


// variables associated with IOT
int iotLight;
float iotTemp;             
float iotPress;                 
char iotdate_time[32];

// cancel alarm 
Timer buzzT;                // timer for buzzer
bool AckPress = false;      // if blue user button has been pressed
Buzzer alarm;               // initiate alarm
microseconds SilenceT = 0s; // will record time elapsed so we know when 60sec have passed before gag order on alarm gets lifted

//lights to state functionality of whether something is working or now. Quick and easy to understand by us
DigitalOut redLED(TRAF_RED1_PIN);   // red led to hightlight buffer error
DigitalOut greenLED(TRAF_GRN1_PIN); // green light to highlight buffer is healthy

//Global Variables
int Lightarray[8] = {0};
float Temparray[8] = {0};   //arrays to hold updated values of sensors
float Pressarray[8] {0};
int c =0;

void buffer::SpaceAllocate(char dt[32], int l, float T, float P){
    buffer* message = mail_box.try_alloc();
        if (message == NULL) {                  //error handling if failed to allocate space in buffer
            redLED = 1;                         // red light comes on
            Watchdog::get_instance().kick();    //kick the watchdog and start 30 seconds from here onwards
                                                //here the 30sec is start fresh and we know where thr problem is
            error("critical error - No space in Buffer. System restart in 30 seconds\n");      //passes  error command
            greenLED = 0;                       // healthy light comes off
        return;
                     }
        int stringLength = strlen(dt);

        for(int i = 0; i <= (stringLength -1); i++){
            message->date_time[i] = dt[stringLength -1-i];
            iotdate_time[stringLength -1-i] = dt[stringLength -1-i];
                }
        date_time[stringLength] = '\0';         //NULL any unsused spaces
        iotdate_time[stringLength] = '\0';
        message-> ldr = l; 
        message-> Temp = T;                     //write passed values to message type buffer*
        message-> Press = P;
        iotLight = l;                           //create some stack based local variables to hold data used later for iot purposes
        iotTemp = T;                            
        iotPress = P;                           
        osStatus stat = mail_box.put(message);  //send message
            if (stat != osOK) {                 //if message fails error is recorded
                        redLED = 1;                     //red light comes on
                        Watchdog::get_instance().kick();//kick the watchdog and start 30 seconds from here onwards
                        error("buffering data failed\n"); // passes  error comman
                        greenLED = 0;                   // healthy light off
                        mail_box.free(message);         // remove message from mail_box
                    return;
            } 
            else{
                greenLED = 1;                   // healthy light on if buffer write successful}
            }
}


void buffer::checkvalues (int l, float T, float P){              // Take values as parameter to check
    ldrVal = l;                                                   
    TempVal = T;                                                 // assign internally for ease of comparing
    PressVal = P;
    SilenceT = buzzT.elapsed_time();

    if (AckPress == false){
        if(ldrVal <= ldralarm_low) {                                 // set high and low using OR condition to encapsulate boundary
            alarm.playTone("C", Buzzer::LOWER_OCTAVE);               // play buzzer to notify threshold breached
            mainQueue.call(printf,"Light Level high Warning!!\n");    // message added to queue to print when pointer points to it (high light level - low eng value)
    }

    if(ldrVal >= ldralarm_high){    //set high and low using or condition to encapsulate boundary
        alarm.playTone("C", Buzzer::HIGHER_OCTAVE);              // play buzzer to notify threshold breached
        mainQueue.call(printf,"Light Level low Warning!!\n");   // message added to queue to print when pointer points to it (low light level - high eng value)
    }

    if(TempVal >= tempalarm_high){
        alarm.playTone("C", Buzzer::LOWER_OCTAVE);
        mainQueue.call(printf,"Temperature high Level Warning!!\n");
    }

     if(TempVal <= tempalarm_low){
        alarm.playTone("C", Buzzer::LOWER_OCTAVE);
        mainQueue.call(printf,"Temperature low Level Warning!!\n");
    }

    if(PressVal >= pressalarm_high){
        alarm.playTone("C", Buzzer::LOWER_OCTAVE);
        mainQueue.call(printf,"Pressure high Level Warning!!\n");
    }

    if(PressVal <= pressalarm_low){
        alarm.playTone("C", Buzzer::LOWER_OCTAVE);
        mainQueue.call(printf,"Pressure low Level Warning!!\n");
    }
    }
    
    if (SilenceT > 60s){                                         // allow cancellation of alarm multiple times
        (AckPress = false);                                    // reset blue button status so can be pressed more than once to turn off alarm
        buzzT.stop();
        buzzT.reset();                                               // reset buzzer so it doesns't ring continuously
        }
}

void buffer::updatearrays(int l, float T, float P){ //updates matrix array
    for(int i = 7; i >0; i --){
        Lightarray[i] = Lightarray[i -1];    
        Temparray[i] = Temparray[i -1];             // pass in values into each space in array decrement style
        Pressarray[i] = Pressarray[i -1];}
        Lightarray[0] = l;
        Temparray[0] = T;                           // pass in the last value into the zeroeth position
        Pressarray[0] = P;
}

void BuzzStop(){
    SilenceT = (buzzT.elapsed_time());
    if(SilenceT == 0s){ //handles the debounce of the button
    buzzT.start();                              // start timer and used above to check if 60s has passed
    alarm.rest();                               // silence alarm
    AckPress = true;                            // bool for if switch has been pressed, in this case it has
    mainQueue.call(printf,"Alarm Silenced for 1 minute.\n");}  //queue message that alarm has been silenced to let the human know
}

void buffer::azureSetpoint(int x, char y, char z){ // Function to update the setpoints within the child class
    if(y == 'L' && z == 'H'){ // If y = L it is light, and z = H it is high
            ldralarm_high = x; // set new value which is passed in
            mainQueue.call(printf, ("Light High Alarm Setpoint changed to: %d\n"), ldralarm_high); // confirm with message
        }
    if(y == 'L' && z == 'L'){ //If y = L it is light, and z = L it is low
            ldralarm_low = x;
            mainQueue.call(printf, ("Light Low Alarm Setpoint changed to: %d\n"), ldralarm_low);
        }
    if(y == 'T' && z == 'H'){ // If y = T it is temp, and z = H it is high.....etc
            tempalarm_high = x;
            mainQueue.call(printf, ("Temperature High Alarm Setpoint changed to: %f\n"), tempalarm_high);
        }
    if(y == 'T' && z == 'L'){
            tempalarm_low = x;
            mainQueue.call(printf, ("Temperature Low Alarm Setpoint changed to: %f\n"), tempalarm_low);
        }
    if(y == 'P' && z == 'H'){ // If y = P it is press, and z = H it is high....etc
            pressalarm_high = x;
            mainQueue.call(printf, ("Pressure High Alarm Setpoint changed to: %f\n"), pressalarm_high);
        }
    if(y == 'P' && z == 'L'){
            pressalarm_low = x;
            mainQueue.call(printf, ("Pressure Low Alarm Setpoint changed to: %f\n"), pressalarm_low);
        }
}


