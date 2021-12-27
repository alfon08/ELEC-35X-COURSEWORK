#ifndef matrix_hpp
#define matrix_hpp
#include "mbed.h"
#include "SPI.H"

class matrix_bar{


private:
SPI matrix_spi;
DigitalOut matrix_spi_cs;            //Chip Select ACTIVE LOW
DigitalOut matrix_spi_oe;           //Output Enable ACTIVE LOW   
void numberLED(int nledLHS, int nledRHS, int row);

public:
void clearMatrix(void);
void matrix_scan(void);
void BarLight(int data); 


matrix_bar() : matrix_spi(PC_12, PC_11, PC_10), matrix_spi_cs(PB_6), matrix_spi_oe(PB_12){
    clearMatrix();
    matrix_scan();
};


};

#endif