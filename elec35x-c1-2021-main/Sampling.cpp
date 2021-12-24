//Group I // Authors Ryan Williams
#include "Sampling.hpp"
#include "mbed.h"
#include "rtos/ThisThread.h"
#include <cstdint>
#include "BMP280_SPI.h"
#include "buffer.hpp"



void Sampling::Sample (){

            data._ldrEng += _ldr.read_u16();
            data._Temp += BMP280.getTemperature();
            data._Press += BMP280.getPressure();
            samples = ++samples;
                        }

void Sampling::UpdateSample(){
            time_t seconds = time(NULL);
            strftime(Samptime_date, 32, "%x: %I:%M:%S %p\n", localtime(&seconds));
            dataAVG = {(data._ldrEng/samples), (data._Temp/samples),  (data._Press/samples), (Samptime_date[32])};
            samples = 0;
            data = {0, 0, 0};
                            }




