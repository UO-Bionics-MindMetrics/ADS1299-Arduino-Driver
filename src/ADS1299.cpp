/* ADS1299.cpp
 * This file contains the implementation of functions found in ADS1299.h
 * It is a driver library for the ADS1299 EEG/EMG/ECG signal aquisition chip from TI
 * 
 * Authors: Mingye Chen
 */

#include "ADS1299.h"
#include "configs.h"

void ADS1299::setup(int _DRDY, int _CS, int RESET_PIN, int mode){
    DRDY = _DRDY;
    CS = _CS;
    RESET_pin = RESET_PIN;
    pinMode(DRDY, INPUT);
    pinMode(CS, OUTPUT);
    pinMode(RESET_pin, OUTPUT);
    
    
    SPI.begin();
    SPI.beginTransaction(SPISettings(1000000,MSBFIRST,SPI_MODE1));
}

//System Commands
void ADS1299::RESET() {
    digitalWrite(CS, LOW);
    SPI.transfer(_RESET);
    delay(10);
//    delay(18.0*tCLK); //must wait 18 tCLK cycles to execute this command (Datasheet, pg. 35)
    digitalWrite(CS, HIGH);
}
void ADS1299::START() {
    digitalWrite(CS, LOW);
    SPI.transfer(_START);
    digitalWrite(CS, HIGH);
}
void ADS1299::STOP() {
    digitalWrite(CS, LOW);
    SPI.transfer(_STOP);
    digitalWrite(CS, HIGH);
}
//Data Read Commands
void ADS1299::RDATAC() {
    digitalWrite(CS, LOW);
    SPI.transfer(_RDATAC);
    digitalWrite(CS, HIGH);
}
void ADS1299::SDATAC() {
    digitalWrite(CS, LOW);
    SPI.transfer(_SDATAC);
    digitalWrite(CS, HIGH);
}
void ADS1299::RDATA() {
    digitalWrite(CS, LOW);
    SPI.transfer(_RDATA);
    digitalWrite(CS, HIGH);
}
void ADS1299::RREG(byte _address) {
    byte opcode1 = _RREG + _address; //001rrrrr; _RREG = 00100000 and _address = rrrrr
    digitalWrite(CS, LOW); //Low to communicated
    SPI.transfer(_SDATAC); //SDATAC
    SPI.transfer(opcode1); //RREG
    SPI.transfer(0x00); //opcode2
    byte data = SPI.transfer(0x00); // returned byte should match default of register map unless edited manually (Datasheet, pg.39)
    printRegisterName(_address);
    Serial.print("0x");
    if(_address<16) Serial.print("0");
    Serial.print(_address, HEX);
    Serial.print(", ");
    Serial.print("0x");
    if(data<16) Serial.print("0");
    Serial.print(data, HEX);
    Serial.print(", ");
    for(byte j = 0; j<8; j++){
        Serial.print(bitRead(data, 7-j), BIN);
        if(j!=7) Serial.print(", ");
    }
    SPI.transfer(_RDATAC); //turn read data continuous back on
    digitalWrite(CS, HIGH); //High to end communication
    Serial.println();
}

void ADS1299::RREG(byte _address, byte _numRegistersMinusOne) {
    byte opcode1 = _RREG + _address; //001rrrrr; _RREG = 00100000 and _address = rrrrr
    digitalWrite(CS, LOW); //Low to communicated
    SPI.transfer(_SDATAC); //SDATAC
    SPI.transfer(opcode1); //RREG
    SPI.transfer(_numRegistersMinusOne); //opcode2
    for(byte i = 0; i <= _numRegistersMinusOne; i++){
        byte data = SPI.transfer(0x00); // returned byte should match default of register map unless previously edited manually (Datasheet, pg.39)
        printRegisterName(i);
        Serial.print("0x");
        if(i<16) Serial.print("0"); //lead with 0 if value is between 0x00-0x0F to ensure 2 digit format
        Serial.print(i, HEX);
        Serial.print(", ");
        Serial.print("0x");
        if(data<16) Serial.print("0"); //lead with 0 if value is between 0x00-0x0F to ensure 2 digit format
        Serial.print(data, HEX);
        Serial.print(", ");
        for(byte j = 0; j<8; j++){
            Serial.print(bitRead(data, 7-j), BIN);
            if(j!=7) Serial.print(", ");
        }
        Serial.println();
    }
    SPI.transfer(_RDATAC); //turn read data continuous back on
    digitalWrite(CS, HIGH); //High to end communication
}

void ADS1299::WREG(byte _address, byte _value) {
    byte opcode1 = _WREG + _address; //001rrrrr; _RREG = 00100000 and _address = rrrrr
    digitalWrite(CS, LOW); //Low to communicated
    SPI.transfer(_SDATAC); //SDATAC
    SPI.transfer(opcode1);
    SPI.transfer(0x00);
    SPI.transfer(_value);
    SPI.transfer(_RDATAC);
    digitalWrite(CS, HIGH); //Low to communicated
    Serial.print("Register 0x");
    Serial.print(_address, HEX);
    Serial.println(" modified.");
}
//Register Read/Write Commands
void ADS1299::getID() {
    digitalWrite(CS, LOW); //Low to communicated
    SPI.transfer(_SDATAC); //SDATAC
    SPI.transfer(_RREG); //RREG
    SPI.transfer(0x00); //Asking for 1 byte
    byte data = SPI.transfer(0x00); // byte to read (hopefully 0b???11110)
    SPI.transfer(_RDATAC); //turn read data continuous back on
    digitalWrite(CS, HIGH); //Low to communicated
    Serial.println(data, BIN);
}

void ADS1299::STARTUP(){
    delay(10);
    digitalWrite(8, HIGH);
    delay(1000);

    // reset pulse
    digitalWrite(RESET_pin,LOW);
    delayMicroseconds(15);
    digitalWrite(RESET_pin, HIGH);
    delay(300);

    SDATAC();
    WREG(CONFIG3, 0xE0);
    delay(500);

    #ifdef SQUARE_SINE_TEST
    init_ADS_4_test(); // square sine test mode
    #elif NORMAL_OPERATION_CONFIG
    init_ADS_4(); // operational mode
    #endif
    
    START();

}

void ADS1299::init_ADS_4(){
    WREG(CONFIG1, 0b11010110);
    WREG(CONFIG2, 0b11000000);
    WREG(CONFIG3, 0b11100100);
    // 0b01100101 - test signal
    // 0b01100000 - normal operation
    WREG(CH1SET, 0b01100001);
    WREG(CH2SET, 0b11010000);
    WREG(CH3SET, 0b11010000);
    WREG(CH4SET, 0b11010000);
    WREG(BIAS_SENSP, 0b00000000);
    WREG(BIAS_SENSN, 0b00000000);
    WREG(GPIO, 0b00000000);
    WREG(MISC1, 0b00000000);
    //WREG(MISC1, 0b00100000);

}

// Square sine wave test
void ADS1299::init_ADS_4_test(){
    WREG(CONFIG1, 0b11010110);
    WREG(CONFIG2, 0b11010000);
    WREG(CONFIG3, 0b11101000);
    // 0b01100101 - test signal
    // 0b01100000 - normal operation
    WREG(CH1SET, 0b01100101);
    WREG(CH2SET, 0b01100101);
    WREG(CH3SET, 0b01100101);
    WREG(CH4SET, 0b01100101);
    WREG(BIAS_SENSP, 0b00001111);
    WREG(BIAS_SENSN, 0b00001111);
    WREG(GPIO, 0b00000000);
}

void ADS1299::RDATA_update(){
    if(digitalRead(DRDY) == LOW){
        //DRDY debug
        //Serial.println("LOW");
        digitalWrite(CS, LOW);
        // RDATA command - ONLY DIFFERENCE BETWEEN THIS FUNC AND THE updateDATA()
        SPI.transfer(_RDATA);

        long output[9];
        uint32_t dataPacket;
        for(int i = 0; i<9; i++){
            for(int j = 0; j<3; j++){
                byte dataByte = SPI.transfer(0x00);
                dataPacket = (dataPacket<<8) | dataByte; // constructing the 24 bit binary
            }
            output[i] = dataPacket;
            dataPacket = 0;
        }
        digitalWrite(CS, HIGH);
        for(int i=0; i<8; i++){
            if(bitRead(output[i],23) == 1){    // convert 3 byte 2's compliment to 4 byte 2's compliment
                output[i] |= 0xFF000000;
            }
            else{
                output[i] &= 0x00FFFFFF;
            }  
        }
        
        // conversions to microvolts 
        
        double outputvolts[9];
        for(int i = 0; i < 9; i++){
            outputvolts[i] = 1000* double(output[i])*2*4.5/24/(pow(2,24));
        }
        
        // changed to get data from 4 channels of ads1299-4
        // i=0;i<9 was original
        for (int i=1;i<5; i++) {
            Serial.print(outputvolts[i]);
            //Serial.print(output[i]);
            if(i!=4) Serial.print("\t");
            
        }
        Serial.println();
    }
}
void ADS1299::RDATA_logger(){
    if(digitalRead(DRDY) == LOW){
        //DRDY debug
        //Serial.println("LOW");
        digitalWrite(CS, LOW);
        // RDATA command - ONLY DIFFERENCE BETWEEN THIS FUNC AND THE updateDATA()
        SPI.transfer(_RDATA);
        
        long output[9];
        uint32_t dataPacket;
        for(int i = 0; i<9; i++){
            for(int j = 0; j<3; j++){
                byte dataByte = SPI.transfer(0x00);
                dataPacket = (dataPacket<<8) | dataByte; // constructing the 24 bit binary
            }
            output[i] = dataPacket;
            dataPacket = 0;
        }
        digitalWrite(CS, HIGH);
        
        datalog[logcounter][0] = output[1];
        //Serial.print(output[1], BIN);
        //Serial.print(",");
        for(int i=0; i<8; i++){
            if(bitRead(output[i],23) == 1){    // convert 3 byte 2's compliment to 4 byte 2's compliment
                output[i] |= 0xFF000000;
            }
            else{
                output[i] &= 0x00FFFFFF;
            }
        }
        datalog[logcounter][1] = output[1];
        //Serial.println(output[1],BIN);
        if(logcounter == LOGSIZE){
            for(int i = 0; i < 100; i++){
                for(int j = 0; j<NUMBEROFCHANNELS*2;j++){
                    Serial.print(datalog[i][j], BIN);
                    Serial.print(",");
                }
            }
            logcounter = 0;
        }
        else{
            logcounter++;
        }
    }
}
// String-Byte converters for RREG and WREG
void ADS1299::printRegisterName(byte _address) {
    if(_address == ID){
        Serial.print("ID, ");
    }
    else if(_address == CONFIG1){
        Serial.print("CONFIG1, ");
    }
    else if(_address == CONFIG2){
        Serial.print("CONFIG2, ");
    }
    else if(_address == CONFIG3){
        Serial.print("CONFIG3, ");
    }
    else if(_address == LOFF){
        Serial.print("LOFF, ");
    }
    else if(_address == CH1SET){
        Serial.print("CH1SET, ");
    }
    else if(_address == CH2SET){
        Serial.print("CH2SET, ");
    }
    else if(_address == CH3SET){
        Serial.print("CH3SET, ");
    }
    else if(_address == CH4SET){
        Serial.print("CH4SET, ");
    }
    else if(_address == CH5SET){
        Serial.print("CH5SET, ");
    }
    else if(_address == CH6SET){
        Serial.print("CH6SET, ");
    }
    else if(_address == CH7SET){
        Serial.print("CH7SET, ");
    }
    else if(_address == CH8SET){
        Serial.print("CH8SET, ");
    }
    else if(_address == BIAS_SENSP){
        Serial.print("BIAS_SENSP, ");
    }
    else if(_address == BIAS_SENSN){
        Serial.print("BIAS_SENSN, ");
    }
    else if(_address == LOFF_SENSP){
        Serial.print("LOFF_SENSP, ");
    }
    else if(_address == LOFF_SENSN){
        Serial.print("LOFF_SENSN, ");
    }
    else if(_address == LOFF_FLIP){
        Serial.print("LOFF_FLIP, ");
    }
    else if(_address == LOFF_STATP){
        Serial.print("LOFF_STATP, ");
    }
    else if(_address == LOFF_STATN){
        Serial.print("LOFF_STATN, ");
    }
    else if(_address == GPIO){
        Serial.print("GPIO, ");
    }
    else if(_address == MISC1){
        Serial.print("MISC1, ");
    }
    else if(_address == MISC2){
        Serial.print("MISC2, ");
    }
    else if(_address == CONFIG4){
        Serial.print("CONFIG4, ");
    }
}