#ifndef sd_hpp
#define sd_hpp
#include "mbed.h"

#include "SDBlockDevice.h"
#include "FATFileSystem.h"
#include "Sampling.hpp"



extern int SDwriteFreq; //ever x samples write to sd 
extern EventQueue mainQueue;

void SDCardSetup();
void SDCardWrite();
void SDCardRead();
void Queue_Read();

#endif