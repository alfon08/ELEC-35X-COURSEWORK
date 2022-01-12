#include "net.hpp"

buffer set(0, 0, 0.0, 0.0);
bool LHAlarmset = false;
bool LLAlarmset = false;
bool THAlarmset = false;
bool TLAlarmset = false;
bool PHAlarmset = false;
bool PLAlarmset = false;
time_t timestamp ;

static bool message_received = false;
bool trace_on = MBED_CONF_APP_IOTHUB_CLIENT_TRACE;
tickcounter_ms_t interval = 100;
IOTHUB_CLIENT_RESULT res;
IOTHUB_DEVICE_CLIENT_HANDLE client_handle = IoTHubDeviceClient_CreateFromConnectionString(
azure_cloud::credentials::iothub_connection_string,
MQTT_Protocol );

bool connect(){ //Function provided with CW - connects to network 
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

void disconnect(){ // function provided with CW - disconnects network 
        LogInfo("disconnecting to the network");
        int ret = _defaultSystemNetwork->disconnect();
            if (ret != 0) {
                LogError("Connection error: %d", ret);
            }
}

bool setTime(){ // function provided with CW - sets time 
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

//function provided from CW template checks connection status and passes error if failed:
static void on_connection_status(IOTHUB_CLIENT_CONNECTION_STATUS result, IOTHUB_CLIENT_CONNECTION_STATUS_REASON reason, void* user_context){
    if (result == IOTHUB_CLIENT_CONNECTION_AUTHENTICATED) {
        LogInfo("Connected to IoT Hub");
    } 
    else {
        LogError("Connection failed, reason: %s", MU_ENUM_TO_STRING(IOTHUB_CLIENT_CONNECTION_STATUS_REASON, reason));
    }
}

// **************************************
// * MESSAGE HANDLER (no response sent) *
// **************************************

// function prvided from CW template - handles message received from IOT
static IOTHUBMESSAGE_DISPOSITION_RESULT on_message_received(IOTHUB_MESSAGE_HANDLE message, void* user_context){
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
            } 
        else {
        led2 = 0;
            }
    return IOTHUBMESSAGE_ACCEPTED;
}

//function provided with CW template - Handles message sent to IOT
static void on_message_sent(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback){
    if (result == IOTHUB_CLIENT_CONFIRMATION_OK) {
        LogInfo("Message sent successfully");
        } 
    else {
        LogInfo("Failed to send message, error: %s",
        MU_ENUM_TO_STRING(IOTHUB_CLIENT_CONFIRMATION_RESULT, result));
        }
}

// ****************************************************
// * COMMAND HANDLER (sends a response back to Azure) *
// ****************************************************

//Function provided with CW template - which provides a callback function on a command received
static int on_method_callback(const char* method_name, const unsigned char* payload, size_t size, unsigned char** response, size_t* response_size, void* userContextCallback){
    const char* device_id = (const char*)userContextCallback;
    //mainQueue.call(printf,"\r\nDevice Method called for device %s\r\n", device_id); //mainqueue prints device from IOT
    //mainQueue.call(printf,"Device Method name:    %s\r\n", method_name); //mainqueue prints method name from IOT
    //mainQueue.call(printf,"Device Method payload: %.*s\r\n", (int)size, (const char*)payload); //mainqueue prints payload name from IOT
    char RESPONSE_STRING[80]; // response string generated
    char *output = NULL;
    int count = 0; // counter 
    int bufcount = 0; // buffer counter
    int flucount = 0; // 
        if(method_name[0] == 'C'){
            for(int i = 0; i < size; i++){ // for loop which iterates throgh chars
                char letter = payload[i]; // exames each char and sets it to letter
                if (letter == 'L'){ // if L then display light matrix
                    y = 'L'; //sets global variable y which is used in main 
                    sprintf(RESPONSE_STRING, "{ \"cmd_res\" : \"Light Level Matrix activated\"}" ); // respond to IOT hub. 
                    }
                if (letter == 'T'){ // T for temp
                    y = 'T';
                    sprintf(RESPONSE_STRING, "{ \"cmd_res\" : \"Temperature Level Matrix activated\"}" );
                    }
                if (letter == 'P'){ //  P for press
                    y = 'P';
                    sprintf(RESPONSE_STRING, "{ \"cmd_res\" : \"Pressure Level Matrix activated\"}" );
                    }  
                    //if (letter == 'l' | letter == 'a' | letter == 't'| letter == 'e'| letter == 's'){
                if (letter == 'l' | letter == 'a' | letter == 't'| letter == 'e'| letter == 's'){
                    count = count +1;
                        if (count == 6){ // counts to 6 to check for all letters
                            mainQueue.call(printf,"Latest values sent check command history\n");
                            sprintf(RESPONSE_STRING, "{ \"cmd_res\" : \"Light %d, Temp %5.2f, Press %5.2f\"}", iotLight, iotTemp, iotPress);
                        }
                    }
                if (letter == 'b' | letter == 'u' | letter == 'f'| letter == 'e'| letter == 'r'){
                    bufcount = bufcount +1;
                        if (bufcount == 6){// counts to 6 to check for all letters
                            sprintf(RESPONSE_STRING, "{ \"cmd_res\" : \"SamplesBuffer %d \"}", numberSamples);
                            mainQueue.call(printf,"Number of samples in the buffer = %d\n", numberSamples);
                        }
                    }
                if (letter == 'f' | letter == 'l' | letter == 'u' | letter == 's' | letter == 'h'){
                    flucount = flucount +1;
                        if (flucount == 5){ // counts to 6 to check for all letters
                            sprintf(RESPONSE_STRING, "{ \"cmd_res\" : \"Buffer emptied\"}" );
                            mainQueue.call(printf,"Buffer emptied\n");
                            Flag_Set2();
                            numberSamples = 0; //Write to the SD card empties the buffer
                        }
                    }
            }   
    }

    if(method_name[0] == 'L'){ // checks method name first line of method name is L this will change light set point
        int num= 0;
        int AlarmSP = 0;
            if(payload[1] == 'l'){ // if payload[1] is l means low if it is h means high
                for(int i = 2; i < size -1; i++){
                    num = ((payload[i]) - 48); // convert ascii to int 
                    AlarmSP = (AlarmSP*10) + num; // add to int 
                } 
                AzureSP_check(AlarmSP,'L', 'L'); // send to function the setpoint and if it is for light and if it is low. 
                sprintf(RESPONSE_STRING, "{ \"AlarmRes\" : \"Low Alarm setpoint to %d\"}", AlarmSP); // respons to IOT hub. 
            }

            if(payload[1] == 'h'){ // same as above but for high setpoint
                for(int i = 2; i < size -1; i++){
                    num = ((payload[i]) - 48);
                    AlarmSP = (AlarmSP*10) + num;
                }
                AzureSP_check(AlarmSP,'L', 'H');
                sprintf(RESPONSE_STRING, "{ \"AlarmRes\" : \"High Alarm setpoint to %d\"}", AlarmSP);
            }
    }

    if((method_name[0] == 'T')){//checks method name first line of method name is T this will change light set point
        int num1= 0;
        int AlarmSP1 = 0;
            if(payload[1] == 'l'){//same as above
                for(int i = 2; i < size -1; i++){
                    num1 = ((payload[i]) - 48);
                    AlarmSP1 = (AlarmSP1*10) + num1;
                } 
            AzureSP_check(AlarmSP1,'T', 'L');
            sprintf(RESPONSE_STRING, "{ \"AlarmRes\" : \"Low Alarm setpoint to %d\"}", AlarmSP1);
            }

            if(payload[1] == 'h'){ //same as above
                for(int i = 2; i < size -1; i++){
                    num1 = ((payload[i]) - 48);
                    AlarmSP1 = (AlarmSP1*10) + num1;
                }
            AzureSP_check(AlarmSP1,'T', 'H');
            sprintf(RESPONSE_STRING, "{ \"AlarmRes\" : \"High Alarm setpoint to %d\"}", AlarmSP1);
            }
    }

    if((method_name[0] == 'P')){ // same as above
        int num2= 0;
        int AlarmSP2 = 0;
            if(payload[1] == 'l'){
                for(int i = 2; i < size -1; i++){
                    num2 = ((payload[i]) - 48);
                    AlarmSP2 = (AlarmSP2*10) + num2;
                } 
                AzureSP_check(AlarmSP2,'P', 'L');
                sprintf(RESPONSE_STRING, "{ \"AlarmRes\" : \"Low Alarm setpoint to %d\"}", AlarmSP2);
            }
            if(payload[1] == 'h'){
                for(int i = 2; i < size -1; i++){
                    num2 = ((payload[i]) - 48);
                    AlarmSP2 = (AlarmSP2*10) + num2;
                }
            AzureSP_check(AlarmSP2,'P', 'H');
            sprintf(RESPONSE_STRING, "{ \"AlarmRes\" : \"High Alarm setpoint to %d\"}", AlarmSP2);
            }
    }
     int status = 200;
     
    //mainQueue.call(printf,"\r\nResponse status: %d\r\n", status); //prints status
    //mainQueue.call(printf,"Response payload: %s\r\n\r\n", RESPONSE_STRING); //prints response payload

    int rlen = strlen(RESPONSE_STRING); // check stringlength and set to rlen
    *response_size = rlen; //create dataype = to rlen
        if ((*response = (unsigned char*)malloc(rlen)) == NULL) { // if mem allocation is = null return -1 
            status = -1;
        }
        else {
            memcpy(*response, RESPONSE_STRING, *response_size); // else memory copy.
        }
    return status;
}

//function provided by CW Template
void iothubrecord() {

    LogInfo("Initializing IoT Hub client"); // prints init
    IoTHub_Init(); //init IOT hub 

    if (client_handle == nullptr) { //error generated if IOT handle is not created. 
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


    IOTHUB_MESSAGE_HANDLE message_handle;
    char message[80];
    while (true) {
            int i = 1;
            ThisThread::flags_wait_all(1); //wait for signal from buffer thread
            ThisThread::flags_clear(1); //clear flags
                if (message_received) {
                    // If we have received a message from the cloud, don't send more messeges
                break;
                }

            sprintf(message, "{ \"LightLevel\" : %d, \"Temperature\" : %5.2f, \"Pressure\" : %5.2f, \"SampTime\" : %s}", iotLight, iotTemp, iotPress, iotdate); //date is in seconds which 
            //is converted by iot to date time // create string message wit data
        

            message_handle = IoTHubMessage_CreateFromString(message); //pass message to handle
            if (message_handle == nullptr) { //if message did not send to handle create error
                LogError("Failed to create message");
                goto cleanup;
            }
            res = IoTHubDeviceClient_SendEventAsync(client_handle, message_handle, on_message_sent, nullptr); // check message was sent
            IoTHubMessage_Destroy(message_handle); // message already copied into the SDK
            if (res != IOTHUB_CLIENT_OK) { // if message did not send no response then create error. 
                LogError("Failed to send message event, error: %d", res);
                goto cleanup;
            }
    }


    while (!message_received) {
        // wait until message is recieved. 
    }

cleanup:
    IoTHubDeviceClient_Destroy(client_handle); // if error happens destroy connection
    IoTHub_Deinit(); // deinitatiate IOT. 
}







