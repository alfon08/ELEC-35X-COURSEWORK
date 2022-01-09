#ifndef sd_hpp
#define sd_hpp
#include "mbed.h"
#include "buffer.hpp"
#include "matrix.hpp"
#include <cstdio>
#include "SDBlockDevice.h"
#include "FATFileSystem.h"
#include "Sampling.hpp"

//Externals:
extern int SDwriteFreq; //ever x samples write to sd 
extern EventQueue mainQueue;

//Functions
void SDCardSetup();
void SDCardWrite();
void SDCardRead();
void Queue_Read();

#endif