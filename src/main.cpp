/* main.cpp
 * This file contains demo code to test the ADS1299
 * 
 * Check configs.h file for configurations that can be set 
 * 
 * Authors: Mingye Chen
 */
#include <Arduino.h>
#include <SPI.h>
#include "ADS1299.h"
ADS1299 ADS;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Serial starting");
  /*
  Pin connections:
  SCK = 13
  MISO (DOUT) = 12
  MOSI (DIN) = 11
  CS = 10; 
  DRDY = 9;
  */
  ADS.setup(9,10,8,1);
  ADS.STARTUP();
  ADS.getID();
  Serial.println("----------------------------------------------");
  //PRINT ALL REGISTERS 
  ADS.RREG(0x00, 0x17);
  Serial.println("----------------------------------------------");
  delay(2000);
}

void loop() {
  #ifdef DATA_LOGGER_MODE
  ADS.RDATA_logger();
  #else
  ADS.RDATA_update();
  #endif
}