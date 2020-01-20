// Visual Micro is in vMicro>General>Tutorial Mode
// 
/*
    Name:       MAX6952Display.ino
    Created:	2020-01-03 12:58:38 AM
    Author:     Wallace Long
*/

// The setup() function runs once each time the micro-controller starts

/*
  MAX6952
  Arduino - MAX6952 (SSOP package)
  5V      - 35, 36, 37
  GND     - 4, 5, 6, 18
  Pin 10 SS   <--> 21 CS
  Pin 11 MOSI <--> 18 DIN
  Pin 12 MISO <--> 20 DOUT
  Pin 13 SCK  <--> 19 CLK
  I've chosen 68k for Rset and 33pF for Cset which works fine so far
*/

#include "MAX6952Display.h"
#include <SPI.h>

const int  CS_pin = 10;      // Pin for chip select

MAX6952Display display(CS_pin);

void setup()
{
	pinMode(LED_BUILTIN, OUTPUT);
	SPI.begin();
	SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));

	display.init();
	
	display.printString("Hi there");
	delay(1000);
	display.printString("This is a");
	delay(1000);
	display.printString("Counter!");
}

void loop()
{
	for(int i = 0; i < 10000; i++) {
		display.printInt(i);
		delay(1000);
	}
}
