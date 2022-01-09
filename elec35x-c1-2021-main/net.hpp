#ifndef net_hpp
#define net_hpp
#include "EthernetInterface.h"
#include "NTPClient.h"
#include <iostream>
#include "uop_msb.h"
#include "mbed.h"
#include "rtos/ThisThread.h"
#include "azure_c_shared_utility/xlogging.h"
#include "certs.h"
#include "iothub.h"
#include "iothub_client_options.h"
#include "iothub_device_client.h"
#include "iothub_message.h"
#include "azure_c_shared_utility/shared_util_options.h"
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/tickcounter.h"
#include "azure_c_shared_utility/xlogging.h"
#include "iothubtransportmqtt.h"
#include "azure_cloud_credentials.h"
#include "buffer.hpp"
#include "mbed.h"
#include "uop_msb.h"
#include "rtos/ThisThread.h"
#include "azure_c_shared_utility/xlogging.h"
#include <chrono>
#include "matrix.hpp"
#include "sd.hpp"

//externals
extern NetworkInterface *_defaultSystemNetwork;
extern int iotLight;
extern float iotTemp;
extern float iotPress;
extern char iotdate[15];
extern char y;
extern char iotdate_time[];
extern int numberSamples;
extern void Flag_Set2();
extern Sampling ldr;
extern DigitalOut led1; 
extern DigitalOut led2;

//functions
bool connect(); // function to connect to network 
void disconnect(); //  function to disconnect
bool setTime(); //  function to set time
void iothubrecord(); 
void sendzure(int l, float t, float p); // function to update child class alarm set points
extern void AzureSP_check(int x, char y, char z); // interlinking function to update child class alarm set points. 

#endif