//Group I // Authors Ryan Williams
#include "Sampling.hpp"

//Gloabl Variables
long secos;
char iotdate[15];
bool samplesupdated = false;

void Sampling::Sample (){               // function to take samples
                    if(samplesupdated == true){
                        data = {0, 0, 0};       // reset data structre
                        samples = 0;            // reset samples
                        samplesupdated = false; // reset for next update
                        }
                    data._ldrEng += _ldr.read_u16();        // read ldr sensor and add to previous within data structure
                    data._Temp += BMP280.getTemperature();  // read temp sensor and add to previous within data structure
                    data._Press += BMP280.getPressure();    // read press sensor and add to previous within data structre
                    samples = ++samples;                    // count how many samples
                   }

void Sampling::UpdateSample(){                      // function to average samples
                    time_t seconds = time(NULL);            // get timestamp
                    secos = (seconds);
                    std::sprintf(iotdate, "%ld000", secos); // write numbers as string
                    strftime(Samptime_date, 32, "%x %X  ", localtime(&seconds)); //records date and time as a string
                    // pass data as a structure for date and time, light, temp and press
                    volatile unsigned int Light = (data._ldrEng/samples);
                    volatile float Temp = (data._Temp/samples);      // take averages over number of samples volatile because they are shared. 
                    volatile float Press = (data._Press/samples);
                    dataAVG = {Light, Temp,  Press, (Samptime_date[32])};   
                    samplesupdated = true;                  // set condition that samples have been updated
                          }




