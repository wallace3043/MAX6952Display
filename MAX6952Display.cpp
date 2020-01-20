//	Author: longwallace@gmail.com
//
//	This library is free software; you can redistribute and/or
//	modify as you please.

extern "C" {
  #include <stdlib.h>
  #include <string.h>
  #include <inttypes.h>
}

#include "MAX6952Display.h"
#include <arduino.h>
#include <SPI.h>

#define READADDRESS 0b10000000
#define DELAY_MICRO 1

enum Registers {
	NoOp			= 0x00,
	Intensity10		= 0x01,
	Intensity32		= 0x02,
	ScanLimit		= 0x03,
	Configuration	= 0x04, 
	UserDefinedFont = 0x05,
	DisplayTest		= 0x07,
	Digit0PlaneP0	= 0x20,
	Digit1PlaneP0	= 0x21,
	Digit2PlaneP0	= 0x22,
	Digit3PlaneP0	= 0x23,
	Digit0PlaneP1	= 0x40,
	Digit1PlaneP1	= 0x41,
	Digit2PlaneP1	= 0x42,
	Digit3PlaneP1	= 0x43,
	WriteD0P0P1		= 0x60,
	WriteD1P0P1		= 0x61,
	WriteD2P0P1		= 0x62,
	WriteD3P0P1		= 0x63
};

enum ConfigurationData {
	//							PXRTEBXS
	NormalOperation			= 0b00000001,
	BlinkRateSelection		= 0b00000100,
	GlobalBlinkEnable		= 0b00001000,
	GlobalBlinkTimingSync	= 0b00010000,
	GlobalClearDigitData	= 0b00100000,
	BlinkPhaseReadback		= 0b10000000
};										
										
MAX6952Display::MAX6952Display(uint8_t pinCS, uint8_t pinDIN /* = 11 */, uint8_t pinDOUT /* = 12 */, uint8_t pinCLK /* = 13 */)
{
	m_pinCS = pinCS;
	m_pinDIN = pinDIN;
	m_pinDOUT = pinDOUT;
	m_pinCLK = pinCLK;
	
	memset(m_intensity, CURRENT_MIN, NUM_DIGITS);
	
	digitalWrite(pinCS, HIGH);
	pinMode(pinCS, OUTPUT);
}

void MAX6952Display::init()
{
	writeDataToAll(Configuration, NormalOperation + GlobalBlinkTimingSync + GlobalClearDigitData);
	writeDataToAll(DisplayTest, 0);
	setIntensity(CURRENT_MIN);
}

void MAX6952Display::clear()
{
	writeDataToAll(Configuration, GlobalClearDigitData);
}

void MAX6952Display::setDigitIntensity(Intensity intensity, uint8_t digitLocation)
{
	m_intensity[digitLocation] = intensity;
	
	writeDataToChip(((digitLocation % 4 < 2)? Intensity10 : Intensity32),
					((digitLocation % 2 == 0)? (m_intensity[digitLocation + 1] <<4) + intensity : (intensity <<4) + m_intensity[digitLocation -1]),
					(digitLocation >>2));
}

void MAX6952Display::setIntensity(Intensity intensity)
{
	memset(m_intensity, intensity, NUM_DIGITS);
		
	writeDataToAll(Intensity10, (intensity <<4) + intensity);
	writeDataToAll(Intensity32, (intensity <<4) + intensity);
}

void MAX6952Display::printChar(char c, uint8_t digitLocation)
{
	writeDataToChip(Digit0PlaneP0 + (digitLocation % 4), c, (digitLocation >>2));
}

void MAX6952Display::printString(char* str, Alignment alignment /* = ALIGN_LEFT */)
{
	clear();
	
	switch(alignment) {
		case ALIGN_LEFT:
			for(int i = 0; i < NUM_DIGITS && str[i] != '\0'; i++)
				printChar(str[i], i);
			break;
		case ALIGN_RIGHT:
			for(int i = 0; i < NUM_DIGITS && str[NUM_DIGITS - i] != '\0'; i--)
				printChar(str[NUM_DIGITS - i], NUM_DIGITS - i);
			break;
		case ALIGN_CENTER:
			int offset = max(0, (NUM_DIGITS - strlen(str)) >>1);
			for(int i = 0; i < NUM_DIGITS && str[i] != '\0'; i++)
				printChar(str[i], i + offset);
			break;
	}
}

void MAX6952Display::PrintNat(unsigned int num, Alignment alignment /* = ALIGN_LEFT */)
{
	// Maximum of NUM_DIGITS digits in num
	while (abs(num) > pow(10, NUM_DIGITS) - 1) {
		num /= 10;
	}
	
	char* str;
}

void MAX6952Display::printInt(int num, Alignment alignment /* = ALIGN_LEFT */)
{
	char* str;
	
	if (num == 0) {
		sprintf(str, "%d", 0);
	} else if (num < 0) {
		// Maximum of NUM_DIGITS digits in num, minus one for the negative sign
		while (abs(num) > pow(10, NUM_DIGITS - 1) - 1) {
			num /= 10;
		}
		printChar('-', 0);
		printInt(abs(num), alignment);
		return;
	} else {
		// Maximum of NUM_DIGITS digits in num
		while (abs(num) > pow(10, NUM_DIGITS - 1)) {
			num /= 10;
		}
		sprintf(str, "%d", num);
	}
	printString(str, alignment);
}

void MAX6952Display::writeDataToChip(byte adr, byte data, uint8_t chip)
{
	digitalWrite(m_pinCS, LOW);
	
	for (int i = NUM_CHIPS; i > chip; i--) {
		SPI.transfer(NoOp);
		SPI.transfer(NoOp);
	}
	SPI.transfer(adr);
	SPI.transfer(data);
	for (int i = chip; i > 0; i--) {
		SPI.transfer(NoOp);
		SPI.transfer(NoOp);
	}
	
	digitalWrite(m_pinCS, HIGH);
	delayMicroseconds(DELAY_MICRO);
}

void MAX6952Display::writeDataToAll(byte adr, byte data)
{
	digitalWrite(m_pinCS, LOW);
	for (int i = 0; i < NUM_CHIPS; i++) {
		SPI.transfer(adr);
		SPI.transfer(data);
	}
	digitalWrite(m_pinCS, HIGH);
	delayMicroseconds(DELAY_MICRO);
}

byte MAX6952Display::readData(byte adr, uint8_t chip)
{
	byte data;
	
	digitalWrite(m_pinCS, LOW);
	SPI.transfer(adr + READADDRESS);
	SPI.transfer(NoOp);
	digitalWrite(m_pinCS, HIGH);
	
	delayMicroseconds(DELAY_MICRO);
	
	digitalWrite(m_pinCS, LOW);
	SPI.transfer(NoOp);
	data = SPI.transfer(NoOp);
	digitalWrite(m_pinCS, HIGH);
	
	return data;
}