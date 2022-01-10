Task 1 (Ryan): Achieved.The device reads light level using the LDR and temperature and pressure using the BMP.
    All the data is encapsulated in Sampling.hpp file. The private data includes a structure with the 
    three samples. In private, there is also included the AnalogIn for the LDR, the EnvSensor to use the
    environmental sensor and a sample counter to count the samples measured. In public, a char which will
    be used to send date and time, a structure with the calculated average values, the constructor and two
    API defined in Sampling.cpp. 
    Sample(): checks if the samples have been updated, if they have, resets them,if not, add the updated 
    values to the previous ones.
    UpdateSample(): calculates the average of all the samples measured.
    Sampling is performed in the highest priority thread.


Task 2 (Ryan): Achieved. Date time and sensor data is written to text file to the SD card. Code in sd.cpp.
    We added a function to read the SD card when you press switch A in the support board to be able to 
    check it if needed.


Task 3 (Ryan): Achieved. buffer.hpp includes the class with all the needed functionality for the FIFO buffer.
   buffer.cpp includes the function to write to the buffer and some other functions for other tasks
   (explained in the other tasks).


Task 4 (Ryan and Anwar): Achieved - Alarm is annunciated and can be muted by the blue button for 1 min. One oversight however, is the 
fact that once the alarm has been silenced no other alarm will be recieved until the 1min has passed. If we had more time this could
be rectified by have a seperate acknowlege function for the different alarms. 

Task 5 (Everyone): Achieved. This project is composed of 5 threads. The thread with the highest priority is t1 which is
   the thread sampling. The other threads all have the same priority. t2 controls the buffer, t3 writes to 
   the SD card, t4 comunicates with iotazure and t6 controls the graph in the LED matrix. Event queues are used
   extensively for ease of operation and to mess with time synchronicity. Signal waits have also been used for this to
   allow for threads to work one after the other when they are called respectively.

Task 6 (Everyone): Achieved. Spin locks have been avoided, printf and LCD writes have been constrained to a minimum and there are 
no waits used. 

Task 7 (Ryan and Alfonso):Achieved (partially). net.cpp includes the functions to send data to azure and to react to some commands. 
   Four string commands are needed in azure.
   Command: performs the task including the LED matrix and the functions latest, buffered and flush
   For the LED matrix write in the command:
   L : plots light level graph
   T : plots temperature
   P : plots pressure
   For the other functions write:
   latest - (Hub recieved data however, time is not recieved due to syntax issues)
   buffer
   flush

   For the other commands:
   LightAlarmSP: sets the new thresholds for the light level
   TemperatureAlarmSP: sets the new thresholds for the temperature
   PressureAlarmSP: sets the new thresholds for the pressure
   For the thresholds, send from azure h and the integer wanted (without spaces or symbols) for the high 
   threshold and l and the integer for low threshold.
	for example to set low value - l50

Task 8 (Ryan and Alfonso): Achieved. Send date, time and sensor measurements to azure. Sensor measurements are ploted in
   graphs using telemetry in azure. It doesnt host information on an internal web page.

Task 9 (Everyone): Achieved. No perceived deadlocks or thread starvations. 

Task 10 (Anwar): Achieved. Although timeouts and other methods may have been preffered, the way we had implemented our code with no
    Mutex locks or writing to shared variables, it did not seem feasible to use them, so we opted for a watchdog. It is started in
    main for 30 seconds, so if it's not kicked wihtin that time, we know the system is hanging so it resents. In the buffer, if we 
    get a critical error, it resets the timer and after 30 seconds, the whole program resets as specified.
    

Task 11 (Ryan and Alfonso): Achieved. in the "Command" command from azure, writting a L, a T or a P will select the graph displayed in the 
   LED matrix. L: light, T:Temperature, P: Pressure. Code in matrix.cpp, matrix.hpp and main (thread 6)


Task 12 (Everyone): Achieved. Everything commented, indented and structured accordignly


####################################################### IMPORTANT FOR TESTING ######################################################

Due to complications related SD and one we were unaware how to resolve, we figure out a certain way the Sd card and code is uploaded
before it can work. 
It goes as follows :
1. Compile and run the code
2. Remove the SD card and reset the nucleo using the black reset button
3. Insert the SD card and reset the nucleo again


Azure information: In Azure we created 7 capabilities. 3 tlemetry: Light Level, Temperature and Pressure
4 commands: Command, LightAlarmSP, TemperatureAlarmSP and PressureAlarmSP


######################################################### Bugs and Errors ##########################################################
Bugs and errors occured during testing:
After certain amount of SD writes the values get corrupted.The bug is very noticeable in the temperature because it reaches 200 and raises 
very quickly. The values in the light  samples we think they get corrupted too, but because of their size (around 30000) the error doesnt
make a huge difference. However, this was resolved by reducing the amount the buffer is holding and doing more sd writes. 
