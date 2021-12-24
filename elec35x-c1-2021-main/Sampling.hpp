//Group I // Authors Ryan Williams
#ifndef Sampling_hpp
#define Sampling_hpp
#include "mbed.h"
#include "rtos/ThisThread.h"
#include <cstdint>
#include "uop_msb.h"

using namespace uop_msb;

class Sampling{

private: //Private Properties
//structure for sampled date
struct Sample{
unsigned int _ldrEng; 
float _Temp;
float _Press;}data;

AnalogIn _ldr; //ldr sensor
EnvSensor BMP280; // environment sensor
unsigned int samples; // samples counter



public: //Constructor and Member functions
char Samptime_date[32]; // char for date and time
//structure for average samples
struct Samps{
unsigned int ldrEngAVG; //Average Engineering Value over x samples
float TempAVG;
float PressAVG;
char Date_Time[32];}dataAVG;

void Sample (); // sample function
void UpdateSample(); // update samples function

//constructor
Sampling(PinName AnalogIn) :_ldr(AnalogIn){
}

};

#endif