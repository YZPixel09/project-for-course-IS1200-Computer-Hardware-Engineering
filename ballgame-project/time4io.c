#include <stdint.h>
#include <pic32mx.h>
#include "mipslab.h"

int getsw(void) {
    // Extract bits 11-8 (SW4 to SW1) from PORTD
    int switches = (PORTD >> 8) & 0xF;
    // Return the switch status as a value 0-15
    return switches;
}

int getbtns(void) {
    // Extract bits 7-5 (BTN4 to BTN2) from PORTD
    int buttons = (PORTD >> 5) & 0x7;
    // Return the button status as a value 0-7
    return buttons;
}