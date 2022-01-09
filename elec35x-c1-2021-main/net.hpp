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
#include <cstring>
#include <string.h>
#include "buffer.hpp"



bool connect();
void disconnect();
bool setTime();
void iothubrecord();
void sendzure(int l, float t, float p);
extern void AzureSP_check(int x, char y, char z);


#endif