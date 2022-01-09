Task 1: Achieved.the device reads light level using the LDR and temperature and pressure using the BMP.
    All the data is encapsulated in Sampling.hpp file. The private data includes a structure with the 
    three samples. In private, there is also included the AnalogIn for the LDR, the EnvSensor to use the
    environmental sensor and a sample counter to count the samples measured. In public, a char which will
    be used to send date and time, a structure with the calculated average values, the constructor and two
    API defined in Sampling.cpp. 
    Sample(): checks if the samples have been updated, if they have, resets them,if not, add the updated 
    values to the previous ones.
    UpdateSample(): calculates the average of all the samples measured.
    Sampling is performed in the highest priority thread.


Task 3: Achieved. buffer.hpp includes the class with all the needed functionality for the FIFO buffer.
   buffer.cpp includes the function to write to the buffer and some other functions for other tasks
   (explained in the other tasks).


Task 4: Achieved 

Task 5: 

Task 6:

Task 7:Achieved. net.cpp includes the fucntions to send data to azure and to react to some commands. 
   Four string commands are needed in github.
   Plot: performs the task including the LED matrix and the functions latest, buffered and flush
   LightAlarmSP: sets the new thresholds for the light level
   TemperatureAlarmSP: sets the new thresholds for the temperature
   PressureAlarmSP: sets the new thresholds for the pressure
   For the thresholds, send from azure h and the integer wanted (without spaces or symbols) for the high 
   threshold and l and the integer for low threshold.

Task 8: Achieved. Send date and time to azure

Task 9: Achieved.

Task 10:

Task 11: in the plot command from azure, writting a L, a T or a P will select the graph displayed in the 
   LED matrix. L: light, T:Temperature, P: Pressure