/*==============================================================================
 Project: Intro-1-Input-Output
 Date:    May 16, 2021
 
 This example UBMP4 input and output program demonstrates pushbutton input, LED
 (bit) output, port latch (byte) output, time delay functions, and simple 'if'
 condition structures.
 
 Additional program analysis and programming activities demonstrate byte output,
 logical condition operators AND and OR, using delay functions to create sound,
 and simulated start-stop button functionality.
==============================================================================*/

#include    "xc.h"              // Microchip XC8 compiler include file
#include    "stdint.h"          // Include integer definitions
#include    "stdbool.h"         // Include Boolean (true/false) definitions

#include    "UBMP4.h"           // Include UBMP4 constants and functions
#include    "Neopic.h"

// TODO Set linker ROM ranges to 'default,-0-7FF' under "Memory model" pull-down.
// TODO Set linker code offset to '800' under "Additional options" pull-down.

// The main function is required, and the program begins executing from here.

#define LEDs 10

unsigned char red[LEDs];
unsigned char green[LEDs];
unsigned char blue[LEDs];

unsigned char sRed = 0;
unsigned char sGreen = 0;
unsigned char sBlue = 0;

unsigned char arrayLength = 4;
unsigned char indexFunction = 0;

int tick = 1;
int ticks_left = 0;
bool isPressed = false;

void epicRGB();
void breathingRGB();
void rgbChooser();
void pulsingRGB();

void (*functions[]) (void) = { epicRGB, breathingRGB, rgbChooser, pulsingRGB };

// i still can't understand this
void hsvToRGB(unsigned char *r, unsigned char *g, unsigned char *b, unsigned char h1, unsigned char s1, unsigned char v1) {
    unsigned char region, p, q, t;
    unsigned int h, s, v, remainder;

    if (s1 == 0)
    {
        *r = v1;
        *g = v1;
        *b = v1;
    }

    // converting to 16 bit to prevent overflow
    h = h1;
    s = s1;
    v = v1;

    region = h / 43;
    remainder = (h - (region * 43)) * 6; 

    p = (v * (255 - s)) >> 8;
    q = (v * (255 - ((s * remainder) >> 8))) >> 8;
    t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

    switch (region)
    {
        case 0:
            *r = v;
            *g = t;
            *b = p;
            break;
        case 1:
            *r = q;
            *g = v;
            *b = p;
            break;
        case 2:
            *r = p;
            *g = v;
            *b = t;
            break;
        case 3:
            *r = p;
            *g = q;
            *b = v;
            break;
        case 4:
            *r = t;
            *g = p;
            *b = v;
            break;
        default:
            *r = v;
            *g = p;
            *b = q;
            break;
    }
}

void epicRGB() {
    for(unsigned char i = 0; i < LEDs; i++) {
        hsvToRGB(&red[i], &green[i], &blue[i], (unsigned char) (tick) + (i * 2), 255, 255);
        red[i] >>= 1;
        green[i] >>= 1;
        blue[i] >>= 1;
    }
    if(ticks_left != 0) ticks_left--;
    tick++;
}


bool pulsingReversed = false;
unsigned char pulsingTick = 0;
unsigned char hue = 0;
void breathingRGB() {
    
    if(!pulsingReversed) {
        pulsingTick++;
        hue++;
        if(pulsingTick == 255) {
            pulsingReversed = true;
        }
    } else {
        pulsingTick--;
        hue++;
        if(pulsingTick == 0) {
            pulsingReversed = false;
        }
    }
    for(unsigned char i = 0; i < LEDs; i++) {
        hsvToRGB(&red[i], &green[i], &blue[i], hue, 255, pulsingTick);
    }
    __delay_ms(4);
}

void rgbChooser() {
    if(SW2 == 0) {
        sRed++;
    } else if(SW3 == 0) {
        sGreen++;
    } else if(SW4 == 0) {
        sBlue++;
    } else if(SW5 == 0) {
        sRed = 0;
        sGreen = 0;
        sBlue = 0;
    }
}

unsigned char ledNum = LEDs;
bool rev = false;
void pulsingRGB() {
    epicRGB();
    if(rev) {
        ledNum++;
        if(ledNum == LEDs) rev = false;
    } else {
        ledNum--;
        if(ledNum == 0) {
            rev = true;
        }
        neopic_fill(LEDs-ledNum, 0, 0, 0);
    }
    neopic_fill_a(ledNum, red, green, blue);
    __delay_ms(15);
}

int main(void)
{
    OSC_config();               // Configure internal oscillator for 48 MHz
    UBMP4_config();             // Configure on-board UBMP4 I/O devices
	
    while(1)
	{   
        if(SW1 == 0 && !isPressed) {
            isPressed = true;
            indexFunction++;
            indexFunction %= arrayLength;
            for(unsigned char i = 0; i < LEDs; i++) {
                red[i] = 0;
                green[i] = 0;
                blue[i] = 0;
            }
            
            ticks_left = 20;
        } else if(SW1 != 0) {
            isPressed = false;
        }
        (*functions[indexFunction])();
        if(indexFunction == 2) {
            neopic_fill(LEDs, sRed, sGreen, sBlue);
        } else if(indexFunction < 2) {
            neopic_fill_a(LEDs, red, green, blue);
        }
        
        __delay_ms(15);
        
        
        // Activate bootloader if SW1 is pressed.
        if(SW1 == 0 && SW2 == 0)
        {
            RESET();
        }
    }
}