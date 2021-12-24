//Group I // Authors Ryan Williams
#ifndef buffer_hpp
#define buffer_hpp
#include "Sampling.hpp"
#include "mbed.h"



class buffer {
    public:
        int ldr;    
        float Temp;
        float Press;
        char date_time[32];
        bool err;

        void SpaceAllocate(char dt[32], int l, float T, float P);
  
    //Constructor
         buffer(char dt[32], int l, float t, float p) {
            
            int stringLength = strlen(dt);
            for(int i = 0; i <= (stringLength -1); i++){
                date_time[i] = dt[stringLength -1-i];
                }
            date_time[stringLength] = '\0';
            ldr = l;
            Temp = t;
            Press = p;          
                                        }
};

extern Mail<buffer, 16> mail_box;

#endif
