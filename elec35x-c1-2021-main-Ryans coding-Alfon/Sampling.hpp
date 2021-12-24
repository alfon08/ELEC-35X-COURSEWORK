//Group I // Authors Ryan Williams
#ifndef Sampling_hpp
#define Sampling_hpp
#include "mbed.h"
#include "rtos/ThisThread.h"
#include <cstdint>
#include "uop_msb.h"

using namespace uop_msb;    //calls the module support board as we are using that

//We will  be using functions which class allows for. variables and methods are hidden
class Sampling{

private: //Private Properties
struct Sample{
unsigned int _ldrEng; //Average Engineering Value over x samples to annoy the mathematicians
float _Temp;
float _Press;}data;
AnalogIn _ldr;
EnvSensor BMP280;
unsigned int samples;



public: //Constructor and Member functions
char Samptime_date[32];
struct Samps{
unsigned int ldrEngAVG; //Average Engineering Value over x samples
float TempAVG;
float PressAVG;
char Date_Time[32];}dataAVG;

void Sample ();
void UpdateSample();

//constructor
Sampling(PinName AnalogIn) :_ldr(AnalogIn){
}

};

#endif