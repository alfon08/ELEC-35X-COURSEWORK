#ifndef main_hpp
#define main_hpp
#include "mbed.h"
#include "mbed.h"   //this has the watchdog function in it
#include "uop_msb.h"
#include "rtos/ThisThread.h"
#include <cstdio>
#include <cstring>
#include <string.h>
#include "Sampling.hpp"
#include <ctype.h>
#include "buffer.hpp"
#include "SDBlockDevice.h"
#include <iostream>
#include "sd.hpp"
#include "net.hpp"
#include "matrix.hpp"

extern int Lightarray[8];
extern float Temparray[8];
extern float Pressarray[8];
extern bool LHAlarmset;
extern bool LLAlarmset;
extern bool TLAlarmset;
extern bool PLAlarmset;
extern int iotLight;
extern void BuzzStop();


void Flag_Set();
void Flag_Set2();
void Flag_Set3();
void AzureSP_check(int x, char y, char z);

#endif
