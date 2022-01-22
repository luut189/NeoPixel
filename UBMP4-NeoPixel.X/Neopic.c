#include    "UBMP4.h"
#include    <xc.h>

void neopic_send(unsigned char col) {
    for(unsigned char bits = 8; bits != 0; bits--) {
        H1OUT = 1;
        if((col & 0b10000000) == 0) {
            H1OUT = 0;
        }
        NOP();
        H1OUT = 0;
        col <<= 1;
    }
}

void neopic_fill(unsigned char leds, unsigned char red, unsigned char green, unsigned char blue) {
    for(; leds != 0; leds--) {
        neopic_send(green);
        neopic_send(red);
        neopic_send(blue);
    }
}

void neopic_fill_a(unsigned char leds, unsigned char red[], unsigned char green[], unsigned char blue[]) {
    unsigned char led = 0;
    for(; led != leds; led++) {
        neopic_send(green[led]);
        neopic_send(red[led]);
        neopic_send(blue[led]);
    }
}

void neopic_fill_single(unsigned char red, unsigned char green, unsigned char blue) {
    neopic_send(green);
    neopic_send(red);
    neopic_send(blue);
}