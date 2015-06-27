/*
	SRF08 - Library for array of TSOPS
	Created by Brian Chen 15/06/2014
	Last Modified by Brian Chen 15/06/2014 7:32pm

	Beta 0.1
*/

#include "Arduino.h"
#include "i2c_t3.h"
#include "SRF08.h"


#define SRF08_I2C_TIMOUT 500

SRF08::SRF08(uint8_t add){
	address = add;
}
void SRF08::getRange(int16_t &range){                // This function gets a ranging from the SRF08
	startRange();
	while (!rangeDone()){
	};	                                   // Wait for ranging to be complete

	Wire.beginTransmission(address);             // start communicating with SRFmodule
	Wire.write(RANGEBYTE);                           // Call the register for start of ranging data
	Wire.endTransmission(I2C_STOP, SRF08_I2C_TIMOUT);

	Wire.requestFrom(address, 2, I2C_STOP, SRF08_I2C_TIMOUT);                // Request 2 bytes from SRF module

	startRequestTime = micros();
	while (Wire.available() < 2){
		if (micros() - startRequestTime > SRF08_I2C_TIMOUT){
			return;
		}
	}                     // Wait for data to arrive
	highByte = Wire.read();                          // Get high byte
	lowByte = Wire.read();                           // Get low byte

	range = (highByte << 8) + lowByte;              
}

void SRF08::startRange(){
	Wire.beginTransmission(address);             // Start communticating with SRF08
	Wire.write(CMD);                                 // Send Command Byte
	Wire.write(0x51);                                // Send 0x51 to start a ranging
	Wire.endTransmission(I2C_STOP, SRF08_I2C_TIMOUT);
	isRanging = true;
}

bool SRF08::rangeDone(){
	uint8_t softRev;
	getSoft(softRev);	

	if (softRev == 255){
		return false;
	}
	return true;
}

bool SRF08::getRangeIfCan(int16_t &range){
	if (rangeDone()){
		isRanging = false;
		Wire.beginTransmission(address);             // start communicating with SRFmodule
		Wire.write(RANGEBYTE);                           // Call the register for start of ranging data
		Wire.endTransmission(I2C_STOP, SRF08_I2C_TIMOUT);

		Wire.requestFrom(address, 2, I2C_STOP, SRF08_I2C_TIMOUT);                // Request 2 bytes from SRF module

		startRequestTime = micros();
		while (Wire.available() < 2){
			if (micros() - startRequestTime > SRF08_I2C_TIMOUT){
				return false;
			}
		}                     // Wait for data to arrive
		highByte = Wire.read();                          // Get high byte
		lowByte = Wire.read();                           // Get low byte

		range = (highByte << 8) + lowByte;
		startRange();
		return true;
	}
	else{
		return false;
	}	
}

void SRF08::getLight(int16_t &lightReading){                                    // Function to get light reading
	Wire.beginTransmission(address);
	Wire.write(LIGHTBYTE);                           // Call register to get light reading
	Wire.endTransmission(I2C_STOP, SRF08_I2C_TIMOUT);

	Wire.requestFrom(address, 1, I2C_STOP, SRF08_I2C_TIMOUT);                // Request 1 byte

	startRequestTime = micros();
	while (Wire.available() < 1){
		if (micros() - startRequestTime > SRF08_I2C_TIMOUT){
			return;
		}
	}    
	lightReading = Wire.read();                     // Get light reading
}

void SRF08::getSoft(uint8_t &softRev){                                     // Function to get software revision
	Wire.beginTransmission(address);             // Begin communication with the SRF module
	Wire.write(CMD);                                 // Sends the command bit, when this bit is read it returns the software revision
	Wire.endTransmission(I2C_STOP, SRF08_I2C_TIMOUT);

	Wire.requestFrom(address, 1, I2C_STOP, SRF08_I2C_TIMOUT);                // Request 1 byte
	
	startRequestTime = micros();
	while (Wire.available() < 1){
		if (micros() - startRequestTime > SRF08_I2C_TIMOUT){
			return;
		}
	}    
	softRev = Wire.read();                      // Get byte
}

bool SRF08::autoGetStartIfCan(int16_t &range){
	if (!isRanging){
		//it's not ranging. Start ranging
		startRange();
		return false;	//return false to show that it hasn't finished ranging
	}
	else{
		return(getRangeIfCan(range));
	}
}

void SRF08::setRange(uint16_t cm){
	Wire.beginTransmission(address);
	Wire.write(CHANGERANGEBYTE);
	Wire.write((uint8_t)(cm * 255 / 1100));
	Wire.endTransmission(I2C_STOP, SRF08_I2C_TIMOUT);
}

void SRF08::setAddress(uint8_t newAddress){	
	Wire.beginTransmission(address);	
	Wire.write(CMD);
	Wire.write(0xA0);	
	Wire.endTransmission(I2C_STOP, SRF08_I2C_TIMOUT);
	
	Wire.beginTransmission(address);
	Wire.write(CMD);
	Wire.write(0xAA);
	Wire.endTransmission(I2C_STOP, SRF08_I2C_TIMOUT);

	Wire.beginTransmission(address);
	Wire.write(CMD);
	Wire.write(0xA5);
	Wire.endTransmission(I2C_STOP, SRF08_I2C_TIMOUT);	

	Wire.beginTransmission(address);
	Wire.write(CMD);
	Wire.write(newAddress);
	Wire.endTransmission(I2C_STOP, SRF08_I2C_TIMOUT);
	address = newAddress;
}