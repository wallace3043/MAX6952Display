//	Author: longwallace@gmail.com
//
//	This library is free software; you can redistribute and/or
//	modify as you please.

#ifndef __MAX6952DISPLAY__
#define __MAX6952DISPLAY__

#include <inttypes.h>
#include <SPI.h>

const int NUM_CHIPS = 2;
const int NUM_DIGITS = 4 * NUM_CHIPS;

enum Intensity {
	// in uA
	CURRENT_2500uA	= 0,
	CURRENT_5000uA,
	CURRENT_7500uA,
	CURRENT_10000uA,
	CURRENT_12500uA,
	CURRENT_15000uA,
	CURRENT_17500uA,
	CURRENT_20000uA,
	CURRENT_22500uA,
	CURRENT_25000uA,
	CURRENT_27500uA,
	CURRENT_30000uA,
	CURRENT_32500uA,
	CURRENT_35000uA,
	CURRENT_37500uA
};

enum Alignment {
	ALIGN_LEFT,
	ALIGN_RIGHT,
	ALIGN_CENTER
};

#define CURRENT_MIN CURRENT_2500uA
#define CURRENT_MAX CURRENT_37500uA
#define ALIGN_CENTRE ALIGN_CENTER

class MAX6952Display {
public:
	//!	Initialize a MAX6952Display object, setting the chip select, clock, and 2 data pins.
	//!	@param pinCS	- The number of the digital pin connected to the CS pin of the module
	//!	@param pinDIN	- The number of the digital pin connected to the DIN pin of the module
	//!	@param pinDOUT	- The number of the digital pin connected to the DOUT pin of the module
	//!	@param pinCLK	- The number of the digital pin connected to the CLK pin of the module
	MAX6952Display(uint8_t pinCS, uint8_t pinDIN = 11, uint8_t pinDOUT = 12, uint8_t pinCLK = 13);
	
	//! Reset and clear a MAX6952Display object to normal operating mode (cleared displays, not blinking)
	//! Must be called after SPI.begin and SPI.beginTransaction.
	void init();
	
	//! Clear the display
	void clear();
	
	//! Set the intensity at the digit location, which is indexed from the left from 0
	void setDigitIntensity(Intensity intensity, uint8_t digitLocation);
	
	//! Set the intensity of all digits
	void setIntensity(Intensity intensity);
	
	//! Display the character at the digit location, which is indexed from the left from 0
	void printChar(char c, uint8_t digitLocation);
	
	//! Display a string, aligned by alignment
	void printString(char* str, Alignment alignment = ALIGN_LEFT);
	
	//! Display an integer, aligned by alignment
	void printInt(int num, Alignment alignment = ALIGN_LEFT);
	
protected:
	
	//! Display a natural (non-zero), aligned by alignment
	void PrintNat(unsigned int num, Alignment alignment = ALIGN_LEFT);
	
	void writeDataToChip(byte adr, byte data, uint8_t chip);
	
	void writeDataToAll(byte adr, byte data);
	
	byte readData(byte adr, uint8_t chip);
	
private:
	uint8_t m_pinCS;
	uint8_t m_pinDIN;
	uint8_t m_pinDOUT;
	uint8_t m_pinCLK;
	
	Intensity m_intensity[NUM_DIGITS];
};

#endif