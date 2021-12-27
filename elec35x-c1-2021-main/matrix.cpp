#include "matrix.hpp"

#define SCAN_RATE_MS   50


// Clear the LED matrix (code by Martin Simpson)
void matrix_bar::clearMatrix(void)
{
    matrix_spi_cs=0;           //CLEAR Matrix
    matrix_spi.write(0x00);//COL RHS
    matrix_spi.write(0x00);//COL LHS
    matrix_spi.write(0x00);//ROX RHS
    matrix_spi_cs=1;
}

void matrix_bar::matrix_scan(void)
{
    for (int j=0;j<=7;j++)  //NB ROW has 3 to 8 decoder so 0 to 7 to Row Select
                            //COLUMNS(COL) are 0 to 255 to select each Byte
    {
        for (int i=1;i<255;i*=2) //Increment LEFT Hand Block of 8
        {
            matrix_spi_cs=0;           //Send Data to Matrix
            matrix_spi.write(0x00);//COL RHS
            matrix_spi.write(i);   //COL LHS
            matrix_spi.write(j);   //ROW RHS
            matrix_spi_cs=1;           //low to high will effectivelly LATCH the Shift register to output
            thread_sleep_for(SCAN_RATE_MS);
        }
        for (int i=1;i<255;i*=2) //Increment RIGHT Hand Block of 8
        {
            matrix_spi_cs=0;           //Send Data to Matrix
            matrix_spi.write(i);   //COL RHS
            matrix_spi.write(0x00);//COL LHS
            matrix_spi.write(j);   //ROW RHS
            matrix_spi_cs=1;           //low to high will effectivelly LATCH the Shift register to output
            thread_sleep_for(SCAN_RATE_MS);
        }
    }
for (int j=0;j<=7;j++)  //NB ROW has 3 to 8 decoder so 0 to 7 to Row Select
                            //COLUMNS(COL) are 0 to 255 to select each Byte
    {
        {
            matrix_spi_cs=0;           //Send Data to Matrix
            matrix_spi.write(0xFF);//COL RHS
            matrix_spi.write(0xFF);   //COL LHS
            matrix_spi.write(j);   //ROW RHS
            matrix_spi_cs=1;           //low to high will effectivelly LATCH the Shift register to output
            thread_sleep_for(SCAN_RATE_MS);
        }
    }
    for (int j=7;j>=0;j--)  //NB ROW has 3 to 8 decoder so 0 to 7 to Row Select
                            //COLUMNS(COL) are 0 to 255 to select each Byte
    {
        {
            matrix_spi_cs=0;           //Send Data to Matrix
            matrix_spi.write(0xFF);//COL RHS
            matrix_spi.write(0xFF);   //COL LHS
            matrix_spi.write(j);   //ROW RHS
            matrix_spi_cs=1;           //low to high will effectivelly LATCH the Shift register to output
            thread_sleep_for(SCAN_RATE_MS);
        }
    }
}

//int zeroLED = 0x00;
//int oneLED = 0x01;

void matrix_bar::numberLED(int nledLHS, int nledRHS, int row){
            matrix_spi_cs=0;           //Send Data to Matrix
            matrix_spi.write(nledRHS);//COL RHS
            matrix_spi.write(nledLHS);   //COL LHS
            matrix_spi.write(row);   //ROW RHS
            matrix_spi_cs=1;           //low to high will effectivelly LATCH the Shift register to output
};



void matrix_bar::BarLight (int data){
    if(data == 0){ //0 or no data
        numberLED(0x00, 0X00, 1);
    }
    if(data <4369){ // one LED on 
        numberLED(0x01, 0x00, 1);
    }
    if(data >=4096 && data< 8738){ // two LEDs on
        numberLED(0x03, 0x00, 1);
    }
    if(data >=4096 && data< 13107){ // three LEDs on
        numberLED(0x0F, 0x00, 1);
    }
    if(data >=8193 && data< 17476){ // four LEDs on 
        numberLED(0x1F, 0x00, 1);
    }
    if(data >=12289 && data< 21845){ // five LEDs on 
        numberLED(0x3F, 0x00, 1);
    }
    if(data >=16385 && data< 26214){ //six LEDs on
        numberLED(0x7F, 0x00, 1);
    }
    if(data >=20481 && data< 24576){ //seven LEDs on
        numberLED(0xFF, 0x00, 1);
    }
    if(data >=24577 && data< 28672){ // eight LEDs on
        numberLED(0xFF, 0x01, 1);
    }
    if(data >=28673 && data< 32768){ // nine LEDs on 
        numberLED(0xFF, 0x03, 1);
    }
    if(data >=32769 && data< 36864){ // 10 LEDs on
        numberLED(0xFF, 0x07, 1);
    }
    if(data >=36865 && data< 40960){ // 11 LEDs on 
        numberLED(0xFF, 0x0F, 1);
    }
    if(data >40961 && data< 45056){ // 12 LEDs on
        numberLED(0xFF, 0x1F, 1);
    }
    if(data >=45057 && data< 53248){ //13 LEDs on
        numberLED(0xFF, 0x3F, 1);
    }
    if(data >=53249 && data< 57344){ //14 LEDs on
        numberLED(0xFF, 0x7F, 1);
    }
    if(data >=57345 && data< 61440){ // 15 LEDs on
        numberLED(0xFF, 0x7F, 1);
    }
}