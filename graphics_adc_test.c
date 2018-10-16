#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <stdio.h>  
#include <stdlib.h>  
#include <stdint.h>
#include <string.h>
#include <time.h>

#include "ssd1306.h"
#include "OledScreen.h"

// compile with g++ -Wall -l wiringPi graphics_test.c OledScreen.cpp -o oled_test

OledScreen oled;

unsigned char initcode[] = {
	// Initialisation sequence
	SSD1306_DISPLAYOFF,                    // 0xAE
	SSD1306_SETLOWCOLUMN,            // low col = 0
	SSD1306_SETHIGHCOLUMN,           // hi col = 0
	SSD1306_SETSTARTLINE,            // line #0
	SSD1306_SETCONTRAST,                   // 0x81
	0xCF,
	0xa1,                                  // setment remap 95 to 0 (?)
	SSD1306_NORMALDISPLAY,                 // 0xA6
	SSD1306_DISPLAYALLON_RESUME,           // 0xA4
	SSD1306_SETMULTIPLEX,                  // 0xA8
	0x3F,                                  // 0x3F 1/64 duty
	SSD1306_SETDISPLAYOFFSET,              // 0xD3
	0x0,                                   // no offset
	SSD1306_SETDISPLAYCLOCKDIV,            // 0xD5
	0xF0,                                  // the suggested ratio 0x80
	SSD1306_SETPRECHARGE,                  // 0xd9
	0xF1,
	SSD1306_SETCOMPINS,                    // 0xDA
	0x12,                                  // disable COM left/right remap
	SSD1306_SETVCOMDETECT,                 // 0xDB
	0x40,                                  // 0x20 is default?
	SSD1306_MEMORYMODE,                    // 0x20
	0x00,                                  // 0x0 act like ks0108
	SSD1306_SEGREMAP,
	SSD1306_COMSCANDEC,
	SSD1306_CHARGEPUMP,                    //0x8D
	0x14,

	// Enabled the OLED panel
	SSD1306_DISPLAYON

};

unsigned char poscode[] = {
   	SSD1306_SETLOWCOLUMN,            // low col = 0
	SSD1306_SETHIGHCOLUMN,           // hi col = 0
	SSD1306_SETSTARTLINE            // line #0
};
 
 
void oled_begin();
void oled_test();
uint32_t mcp3008_read(uint8_t);
 
int main(void)
{
    // setup GPIO, this uses actual BCM pin numbers 
    wiringPiSetupGpio();
    oled_begin();
    oled_test();
    return 0;
}
 
void oled_begin()
{       
    pinMode (5, OUTPUT) ;
    pinMode (6, OUTPUT) ;
    wiringPiSPISetup(0, 4*1000*1000);
    wiringPiSPISetup(1, 4*1000*1000);
    
    // reset
    digitalWrite(6,  LOW) ;
    delay(50);
    digitalWrite(6,  HIGH) ;
    
    // init
    digitalWrite(5, LOW);
    wiringPiSPIDataRW(0, initcode, 28);
}

// draw random crap
void oled_test()
{ 
    int i,j,k;
    char str[128];
       
    
    for(k=0;k<10000;k++)
    {
        digitalWrite(5, LOW);
        wiringPiSPIDataRW(0, poscode, 3);
        digitalWrite(5, HIGH);
        wiringPiSPIDataRW(0, oled.pix_buf, 1024);
        
        // write extra for 64x132 oled
        //wiringPiSPIDataRW(0, oled.pix_buf, 32);
 
        oled.clear();
        sprintf(str, "%d", mcp3008_read(1));
	    oled.println(str, 10, 10, 32, 1);
        delay(33);
    }
}

// read a channel
uint32_t mcp3008_read(uint8_t adcnum)
{ 
    unsigned int commandout = 0;
    unsigned int adcout = 0;

    commandout = adcnum & 0x3;  // only 0-7
    commandout |= 0x18;     // start bit + single-ended bit

    uint8_t spibuf[3];

    spibuf[0] = commandout;
    spibuf[1] = 0;
    spibuf[2] = 0;

    wiringPiSPIDataRW(1, spibuf, 3);    

    return ((spibuf[1] << 8) | (spibuf[2])) >> 4;
    
}
