#include "sd.hpp"
#include "buffer.hpp"

SDBlockDevice sdc(PB_5, PB_4, PB_3, PF_3);


void SDCardSetup(){
    //for(int z = 0;z >2;z++) { //loop to init sd card sometimes doesnt always do it first time??
    int err;
    err=sdc.init();
    if ( 0 != err) {
        mainQueue.call(printf,"Init failed %d\n",err);}
    FATFileSystem fs("sd", &sdc);
    FILE *fp = fopen("/sd/test.txt","w");
    if(fp == NULL) {
        error("Could not open file for write\n");
        sdc.deinit();
    } else {
        fprintf(fp, "Log files:\n" );
        fclose(fp);
        mainQueue.call(printf,"SD Card setup\n");
    }
}


//Thread 3
void SDCardWrite(){
    FATFileSystem fs("sd", &sdc);
    FILE *fp = fopen("/sd/test.txt","a");
    int p;
    int r =0;
    while(true){
    if(r == -1){
        printf("repeat");
        r =0;
    }
    if(fp == NULL) {
        printf("Error\n");
        error("Could not open or find file for read\n");
        sdc.deinit();
    } else {
        ThisThread::flags_wait_any(1); 
        ThisThread::flags_clear(1);
        FILE *fp = fopen("/sd/test.txt","a"); 
        for(p = 0; p<(SDwriteFreq); p++){
        buffer* payload;
        payload = mail_box.try_get(); 
        buffer msg(payload->date_time, payload->ldr, payload->Temp, payload->Press);
        mail_box.free(payload);
        fprintf(fp, "%s", msg.date_time); //to be updated wit
        fprintf(fp, "ldr value: %d\t", msg.ldr); //to be updated wit
        fprintf(fp, "Temp value: %.2f\t", msg.Temp);
        fprintf(fp, "Press value: %.2f\t\n", msg.Press);
        }
        mainQueue.call(printf, "sd card write complete\n");
        fclose(fp);
        p = 0;
        r++;
        }
        
    }
}

void SDCardRead(){

             FILE *fp = fopen("/sd/test.txt","r");
             char buff[80]; 
             while (!feof(fp)) {
             fgets(buff, 79, fp);
             printf("%s\n", buff);}
             fclose(fp);
             mainQueue.call(printf, "sd card Read complete\n");
}

void Queue_Read(){
     mainQueue.call(&SDCardRead);
                }