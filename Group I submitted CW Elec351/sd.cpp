#include "sd.hpp"

SDBlockDevice sdc(PB_5, PB_4, PB_3, PF_3);

void SDCardSetup(){
    int err;
    err=sdc.init(); //creats an instance of sdc and returns error int
        if ( 0 != err) { //semephore to check if init has failed
            mainQueue.call(printf,"Init failed %d\n",err);} // print error message

        FATFileSystem fs("sd", &sdc); // creates file system
        FILE *fp = fopen("/sd/test.txt","w"); // open file within file structure sd return as fp

        if(fp == NULL) { //if file doesnt open
            error("Could not open file for write\n"); //error
            sdc.deinit(); // deinit the sd file
        } 
        else {
            fprintf(fp, "Log files:\n" ); //print start of file 
            fclose(fp); // close file
            mainQueue.call(printf,"SD Card setup\n"); // message to inform sd card setup
        }
}


//Thread 3
void SDCardWrite(){
    FATFileSystem fs("sd", &sdc); //address of file
    FILE *fp = fopen("/sd/test.txt","a"); // open file
    int p; // varible for for loop
    //int r =0; 
    while(true){
    // if(r == -1){
    //     printf("repeat");
    //     r =0;
    // }
        if(fp == NULL) { //if file fails to open
            error("Could not open or find file for read\n"); // record error
            sdc.deinit(); //  deinit sdc
            }   
        else {
            ThisThread::flags_wait_any(1); // wait for flag set by buffer thread
            ThisThread::flags_clear(1);// clear threads
            FILE *fp = fopen("/sd/test.txt","a"); //Open file to  write
                for(p = 0; p<(SDwriteFreq); p++){ // for loop to go through number of samples recorded
                    buffer* payload; // create instance of buffer
                    payload = mail_box.try_get(); // try to put payload size in mail box
                    buffer msg(payload->date_time, payload->ldr, payload->Temp, payload->Press); //create message for mailbox
                    mail_box.free(payload); // free payload 
                    fprintf(fp, "%s", msg.date_time); //Print date and time in sd
                    fprintf(fp, "ldr value: %d\t", msg.ldr); //print readings in sd
                    fprintf(fp, "Temp value: %.2f\t", msg.Temp);
                    fprintf(fp, "Press value: %.2f\t\n", msg.Press);
                }
        mainQueue.call(printf, "sd card write complete\n"); // inform sd card complete
        fclose(fp); //  clsoe file 
        p = 0; // reset variable
        //r++;
        //mutex2.unlock();
            }
        }   
}

void SDCardRead(){ //Reads SD card //test to show measurements are being stored in sd
             FILE *fp = fopen("/sd/test.txt","r"); // opens file
             char buff[80]; // creates string char 
             while (!feof(fp)) { // while loop till end of file
             fgets(buff, 79, fp); // gets date from sd and stores in buff
             printf("%s\n", buff);} // prints stored data
             fclose(fp); // closes file 
             mainQueue.call(printf, "sd card Read complete\n"); //informs read is complete
}

void Queue_Read(){ 
    //Wait_us(10000);                 //switch debounce
     mainQueue.call(&SDCardRead);   //queue read sd card so it doensn't clash with anything
                }