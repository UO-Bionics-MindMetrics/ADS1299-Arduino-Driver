/* ADS1299.h
 * Header file for ADS1299.cpp
 * Driver library for ADS1299 from TI
 * 
 * Authors: Mingye Chen
 */
#ifndef ADS1299_h
#define ADS1299_h

#include <stdio.h>
#include <Arduino.h>
#include <SPI.h>
#include "Definitions.h"
#include "configs.h"

class ADS1299 {
public:
    int mode = 1;
    int DRDY, CS, RESET_pin;
    int logcounter=0;
    long datalog[LOGSIZE][NUMBEROFCHANNELS*2];
    float VREF;
    void setup(int _DRDY, int _CS, int RESET_PIN, int mode); // mode is either RDATAC - 0 or RDATA - 0

    void RESET();
    void START();
    void STOP();
    
    void RDATAC();
    void SDATAC();
    void RDATA();

    void getID();
    void RREG(byte _address);
    void RREG(byte _address, byte _numRegistersMinusOne); //to read multiple consecutive registers (Datasheet, pg. 38)
    
    void printRegisterName(byte _address);
    
    void WREG(byte _address, byte _value); //
    void WREG(byte _address, byte _value, byte _numRegistersMinusOne); //
    
    void updateData(); // RDATAC
    void RDATA_update(); // RDATA
    void RDATA_logger();
    void STARTUP(); // Startup routine
    void init_ADS_4();// initialize configs for 4 channel version
    void init_ADS_8();// initialize configs for 8 channel version
    void init_ADS_4_test();
};

#endif