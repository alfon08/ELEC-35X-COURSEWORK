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

int f = 0x00 - 0x04;

void matrix_bar::BarLight (int data, int row){
    if(data == 0){ //0 or no data
        numberLED(0x00, 0X00, row);
    }
    if(data < 3500){ // one LED on 
        numberLED(0x01, 0x00, row);
    }
    if(data >=3500 && data< 7000){ // two LEDs on
        numberLED(0x03, 0x00, row);
    }
    if(data >=7000 && data< 10500){ // three LEDs on
        numberLED(0x07, 0x00, row);
    }
    if(data >=10500 && data< 14000){ // four LEDs on 
        numberLED(0x0F, 0x00, row);
    }
    if(data >=14000 && data< 17500){ // five LEDs on 
        numberLED(0x1F, 0x00, row);
    }
    if(data >=17500 && data< 21000){ //six LEDs on
        numberLED(0x3F, 0x00, row);
    }
    if(data >=21000 && data< 24500){ //seven LEDs on
        numberLED(0x7F, 0x00, row);
    }
    if(data >=24500 && data< 28000){ // eight LEDs on
        numberLED(0xFF, 0x00, row);
    }
    if(data >=28000 && data< 31500){ // nine LEDs on 
        numberLED(0xFF, 0x01, row);
    }
    if(data >=31500 && data< 35000){ // 10 LEDs on
        numberLED(0xFF, 0x03, row);
    }
    if(data >=35000 && data< 38500){ // 11 LEDs on 
        numberLED(0xFF, 0x07, row);
    }
    if(data >38500 && data< 42000){ // 12 LEDs on
        numberLED(0xFF, 0x0F, row);
    }
    if(data >=42000 && data< 45500){ //13 LEDs on
        //numberLED(0xFF, 0x3F, row);
        numberLED(0xFF, 0x1f, row);
    }
    if(data >=45500 && data< 49000){ //14 LEDs on
        numberLED(0xFF, 0x3f, row);
    }
    if(data >=49000 && data< 52500){ // 15 LEDs on
        numberLED(0xFF, 0x7F, row);
    }
    if(data >=56000){ // 15 LEDs on
        numberLED(0xFF, 0xFF, row);
    }
}
void matrix_bar::BarTemp (float data, int row){
    if(data == 0){ //0 or no data
        numberLED(0x00, 0X00, row);
    }
    if(data < 2.5){ // one LED on 
        numberLED(0x01, 0x00, row);
    }
    if(data >=2.5 && data< 5){ // two LEDs on
        numberLED(0x03, 0x00, row);
    }
    if(data >=5 && data< 7.5){ // three LEDs on
        numberLED(0x07, 0x00, row);
    }
    if(data >=7.5 && data< 10){ // four LEDs on 
        numberLED(0x0F, 0x00, row);
    }
    if(data >=10 && data< 12.5){ // five LEDs on 
        numberLED(0x1F, 0x00, row);
    }
    if(data >=12.5 && data< 15){ //six LEDs on
        numberLED(0x3F, 0x00, row);
    }
    if(data >=15 && data< 17.5){ //seven LEDs on
        numberLED(0x7F, 0x00, row);
    }
    if(data >=17.5 && data< 20){ // eight LEDs on
        numberLED(0xFF, 0x00, row);
    }
    if(data >=20 && data< 22.5){ // nine LEDs on 
        numberLED(0xFF, 0x01, row);
    }
    if(data >=22.5 && data< 25){ // 10 LEDs on
        numberLED(0xFF, 0x03, row);
    }
    if(data >=25 && data< 27.5){ // 11 LEDs on 
        numberLED(0xFF, 0x07, row);
    }
    if(data >27.5 && data< 30){ // 12 LEDs on
        numberLED(0xFF, 0x0F, row);
    }
    if(data >=30 && data< 32.5){ //13 LEDs on
        //numberLED(0xFF, 0x3F, row);
        numberLED(0xFF, 0x1f, row);
    }
    if(data >=32.5 && data< 35){ //14 LEDs on
        numberLED(0xFF, 0x3f, row);
    }
    if(data >=35 && data< 37.5){ // 15 LEDs on
        numberLED(0xFF, 0x7F, row);
    }
    if(data >=37.5){ // 15 LEDs on
        numberLED(0xFF, 0xFF, row);
    }
}

void matrix_bar::BarPres (float data, int row){
    if(data < 300){ //0 or no data
        numberLED(0x00, 0X00, row);
    }
    if(data >=300 && data< 350){ // one LED on 
        numberLED(0x01, 0x00, row);
    }
    if(data >=350 && data< 400){ // two LEDs on
        numberLED(0x03, 0x00, row);
    }
    if(data >= 400 && data< 450){ // three LEDs on
        numberLED(0x07, 0x00, row);
    }
    if(data >= 450 && data< 500){ // four LEDs on 
        numberLED(0x0F, 0x00, row);
    }
    if(data >=500 && data< 550){ // five LEDs on 
        numberLED(0x1F, 0x00, row);
    }
    if(data >=550 && data< 600){ //six LEDs on
        numberLED(0x3F, 0x00, row);
    }
    if(data >=600 && data< 650){ //seven LEDs on
        numberLED(0x7F, 0x00, row);
    }
    if(data >=650 && data< 700){ // eight LEDs on
        numberLED(0xFF, 0x00, row);
    }
    if(data >=700 && data< 750){ // nine LEDs on 
        numberLED(0xFF, 0x01, row);
    }
    if(data >=750 && data< 800){ // 10 LEDs on
        numberLED(0xFF, 0x03, row);
    }
    if(data >= 800 && data< 850){ // 11 LEDs on 
        numberLED(0xFF, 0x07, row);
    }
    if(data >850 && data< 900){ // 12 LEDs on
        numberLED(0xFF, 0x0F, row);
    }
    if(data >=900 && data< 950){ //13 LEDs on
        //numberLED(0xFF, 0x3F, row);
        numberLED(0xFF, 0x1f, row);
    }
    if(data >=950 && data< 1000){ //14 LEDs on
        numberLED(0xFF, 0x3f, row);
    }
    if(data >=1050 && data< 1100){ // 15 LEDs on
        numberLED(0xFF, 0x7F, row);
    }
    if(data >=1100){ // 15 LEDs on
        numberLED(0xFF, 0xFF, row);
    }
}