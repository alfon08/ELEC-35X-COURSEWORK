#include "net.hpp"
#include "mbed.h"
#include "uop_msb.h"
#include "rtos/ThisThread.h"
#include "azure_c_shared_utility/xlogging.h"
#include <chrono>
#include <cstring>
#include <string.h>
#include "buffer.hpp"
#include "matrix.hpp"
#include "sd.hpp"
//NetworkInterface* netIF;
extern int iotLight;
extern float iotTemp;
extern float iotPress;
extern char iotdate[15];
extern void matrix_display(char y);
extern char iotdate_time[];
extern int numberSamples;

extern NetworkInterface *_defaultSystemNetwork;
time_t timestamp ;
extern Sampling ldr;
static bool message_received = false;
    bool trace_on = MBED_CONF_APP_IOTHUB_CLIENT_TRACE;
    tickcounter_ms_t interval = 100;
    IOTHUB_CLIENT_RESULT res;

        IOTHUB_DEVICE_CLIENT_HANDLE client_handle = IoTHubDeviceClient_CreateFromConnectionString(
        azure_cloud::credentials::iothub_connection_string,
        MQTT_Protocol );

bool connect(){
        LogInfo("Connecting to the network");

    _defaultSystemNetwork = NetworkInterface::get_default_instance();
    if (_defaultSystemNetwork == nullptr) {
        LogError("No network interface found");
        return false;
    }

    int ret = _defaultSystemNetwork->connect();
    if (ret != 0) {
        LogError("Connection error: %d", ret);
        return false;
    }
    LogInfo("Connection success, MAC: %s", _defaultSystemNetwork->get_mac_address());
    return true;
}

void disconnect(){
        LogInfo("disconnecting to the network");

    int ret = _defaultSystemNetwork->disconnect();
    if (ret != 0) {
        LogError("Connection error: %d", ret);
    }
}


bool setTime(){

        LogInfo("Getting time from the NTP server");

    NTPClient ntp(_defaultSystemNetwork);
    ntp.set_server("time.google.com", 123);
    time_t timestamp = ntp.get_timestamp();
    if (timestamp < 0) {
        LogError("Failed to get the current time, error: %ud", timestamp);
        return false;
    }
    LogInfo("Time: %s", ctime(&timestamp));
    set_time(timestamp);
    return true;
}

void matrixUpdate(){
    

}

extern DigitalOut led1; 
extern DigitalOut led2;



static void on_connection_status(IOTHUB_CLIENT_CONNECTION_STATUS result, IOTHUB_CLIENT_CONNECTION_STATUS_REASON reason, void* user_context)
{
    if (result == IOTHUB_CLIENT_CONNECTION_AUTHENTICATED) {
        LogInfo("Connected to IoT Hub");
    } else {
        LogError("Connection failed, reason: %s", MU_ENUM_TO_STRING(IOTHUB_CLIENT_CONNECTION_STATUS_REASON, reason));
    }
}

// **************************************
// * MESSAGE HANDLER (no response sent) *
// **************************************
static IOTHUBMESSAGE_DISPOSITION_RESULT on_message_received(IOTHUB_MESSAGE_HANDLE message, void* user_context)
{
    LogInfo("Message received from IoT Hub");

    const unsigned char *data_ptr;
    size_t len;
    if (IoTHubMessage_GetByteArray(message, &data_ptr, &len) != IOTHUB_MESSAGE_OK) {
        LogError("Failed to extract message data, please try again on IoT Hub");
        return IOTHUBMESSAGE_ABANDONED;
    }

    message_received = true;
    LogInfo("Message body: %.*s", len, data_ptr);

    if (strncmp("true", (const char*)data_ptr, len) == 0) {
        led2 = 1;
    } else {
        led2 = 0;
    }

    return IOTHUBMESSAGE_ACCEPTED;
}

static void on_message_sent(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback)
{
    if (result == IOTHUB_CLIENT_CONFIRMATION_OK) {
        LogInfo("Message sent successfully");
    } else {
        LogInfo("Failed to send message, error: %s",
            MU_ENUM_TO_STRING(IOTHUB_CLIENT_CONFIRMATION_RESULT, result));
    }
}

// ****************************************************
// * COMMAND HANDLER (sends a response back to Azure) *
// ****************************************************
static int on_method_callback(const char* method_name, const unsigned char* payload, size_t size, unsigned char** response, size_t* response_size, void* userContextCallback)
{
    const char* device_id = (const char*)userContextCallback;


    printf("\r\nDevice Method called for device %s\r\n", device_id);
    printf("Device Method name:    %s\r\n", method_name);
    printf("Device Method payload: %.*s\r\n", (int)size, (const char*)payload);
    char RESPONSE_STRING[80];
    char *output = NULL;
    int count = 0;
    int bufcount = 0;
    int flucount = 0;
    char b[] = "late";
    int payloadsize = sizeof(payload);
    for(int i = 0; i < payloadsize; i++){
        const char letter = payload[i];
        if (letter == 'L'){
            matrix_display('L');
            printf("%d", payloadsize);
            sprintf(RESPONSE_STRING, "{ \"cmd_res\" : \"Light Level Matrix activated\"}" );
        }
        if (letter == 'T'){
            matrix_display('T');
            sprintf(RESPONSE_STRING, "{ \"cmd_res\" : \"Temperature Level Matrix activated\"}" );
        }
        if (letter == 'P'){
            matrix_display('P');
            sprintf(RESPONSE_STRING, "{ \"cmd_res\" : \"Pressure Level Matrix activated\"}" );
        }  

        
        if (letter == 'l' | letter == 'a' | letter == 't'){
            count = count +1;
                if (count == 3){
                    sprintf(RESPONSE_STRING, "{ \"LightLevel\" : %d, \"Temperature\" : %5.2f, \"Pressure\" : %5.2f,}", iotLight, iotTemp, iotPress);
                    count = 0;
                }
        }
        if (letter == 'b' | letter == 'u' | letter == 'f'){
            bufcount = bufcount +1;
                if (bufcount == 3){
                    sprintf(RESPONSE_STRING, "{ \"SamplesBuffer\" : %i,}", numberSamples);
                    printf("Number of samples in the buffer = %i", numberSamples);
                    bufcount = 0;
                }
        }
        if (letter == 'f' | letter == 'l' | letter == 'u'){
            flucount = flucount +1;
                if (flucount == 3){
                    
                    SDCardWrite(); //Write the values from the buzzer to the SD card
                    printf("Buffer emptied");
                    flucount = 0;
                }
        }
    }


    

    // output = strstr(a,b);
    //     if(output != NULL){
    //         printf("get latest");
    //     }

    // }

     int status = 200;
     
    printf("\r\nResponse status: %d\r\n", status);
    printf("Response payload: %s\r\n\r\n", RESPONSE_STRING);

    int rlen = strlen(RESPONSE_STRING);
    *response_size = rlen;
    if ((*response = (unsigned char*)malloc(rlen)) == NULL) {
        status = -1;
    }
    else {
        memcpy(*response, RESPONSE_STRING, *response_size);
    }
    return status;
}

void iothubrecord() {

    LogInfo("Initializing IoT Hub client");
    IoTHub_Init();

    if (client_handle == nullptr) {
        LogError("Failed to create IoT Hub client handle");
        goto cleanup;
    }

    // Enable SDK tracing
    res = IoTHubDeviceClient_SetOption(client_handle, OPTION_LOG_TRACE, &trace_on);
    if (res != IOTHUB_CLIENT_OK) {
        LogError("Failed to enable IoT Hub client tracing, error: %d", res);
        goto cleanup;
    }

    // Enable static CA Certificates defined in the SDK
    res = IoTHubDeviceClient_SetOption(client_handle, OPTION_TRUSTED_CERT, certificates);
    if (res != IOTHUB_CLIENT_OK) {
        LogError("Failed to set trusted certificates, error: %d", res);
        goto cleanup;
    }

    // Process communication every 100ms
    res = IoTHubDeviceClient_SetOption(client_handle, OPTION_DO_WORK_FREQUENCY_IN_MS, &interval);
    if (res != IOTHUB_CLIENT_OK) {
        LogError("Failed to set communication process frequency, error: %d", res);
        goto cleanup;
    }

    // set incoming message callback
    res = IoTHubDeviceClient_SetMessageCallback(client_handle, on_message_received, nullptr);
    if (res != IOTHUB_CLIENT_OK) {
        LogError("Failed to set message callback, error: %d", res);
        goto cleanup;
    }

    // Set incoming command callback
    res = IoTHubDeviceClient_SetDeviceMethodCallback(client_handle, on_method_callback, nullptr);
    if (res != IOTHUB_CLIENT_OK) {
        LogError("Failed to set method callback, error: %d", res);
        goto cleanup;
    }

    // Set connection/disconnection callback
    res = IoTHubDeviceClient_SetConnectionStatusCallback(client_handle, on_connection_status, nullptr);
    if (res != IOTHUB_CLIENT_OK) {
        LogError("Failed to set connection status callback, error: %d", res);
        goto cleanup;
    }

    // Send ten message to the cloud (one per second)
    // or until we receive a message from the cloud
    IOTHUB_MESSAGE_HANDLE message_handle;
    char message[80];
    while (true) {
            int i = 1;
            ThisThread::flags_wait_all(1); 
            ThisThread::flags_clear(1);

        if (message_received) {
            // If we have received a message from the cloud, don't send more messeges
            break;
        }

        sprintf(message, "{ \"LightLevel\" : %d, \"Temperature\" : %5.2f, \"Pressure\" : %5.2f, \"SampTime\" : %s}", iotLight, iotTemp, iotPress, iotdate);
        //LogInfo("Sending: \"%s\"", message);

        message_handle = IoTHubMessage_CreateFromString(message);
        if (message_handle == nullptr) {
            LogError("Failed to create message");
            goto cleanup;
        }

        res = IoTHubDeviceClient_SendEventAsync(client_handle, message_handle, on_message_sent, nullptr);
        IoTHubMessage_Destroy(message_handle); // message already copied into the SDK

        if (res != IOTHUB_CLIENT_OK) {
            LogError("Failed to send message event, error: %d", res);
            goto cleanup;
        }

    }

    // If the user didn't manage to send a cloud-to-device message earlier,
    // let's wait until we receive one
    while (!message_received) {
        // Continue to receive messages in the communication thread
        // which is internally created and maintained by the Azure SDK.
        //sleep();
    }

cleanup:
    IoTHubDeviceClient_Destroy(client_handle);
    IoTHub_Deinit();
}





