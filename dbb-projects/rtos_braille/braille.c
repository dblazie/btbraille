/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
//#include "debug.h"
#define CLOCK_DELAY_MS .01
#ifndef LED_DELAY_MS
#define LED_DELAY_MS 1000
#endif

#define clock 18
#define data 19
#define strobe 20
#define wait 1

#include "braille.h"


// give a clock pulse 
void pulse_clock() {
    gpio_put(clock,1);
        sleep_ms(CLOCK_DELAY_MS);
    gpio_put(clock,0);
        sleep_ms(CLOCK_DELAY_MS);
}

// give a strobe pulse 
void do_strobe() {
        sleep_ms(CLOCK_DELAY_MS);
    gpio_put(strobe,1);
        sleep_ms(CLOCK_DELAY_MS);
    gpio_put(strobe,0);
}

void send_byte(int x) {
    int c = x;
    int j = 0;
    if (x&0x1) j |= 0x10;
    if (!(x&0x2)) j |= 0x20;
    if (x&0x4) j |= 0x40;
    if (!(x&0x8)) j |= 0x80;
    if (x&0x10) j = j|0x01;
    if (!(x&0x20)) j |= 0x2;
    if (x&0x40) j |= 0x4;
    if (!(x&0x80)) j |= 0x8;
    
    for (int i = 0; i <= 7; i++) {
//	debug_log("testing c, i, j = %d %d %0x %0x\n", c, i, j, j&1);
	gpio_put(data, j&1);  // send the right most bit, but mod it with 2 because every other bit is inverted by KGS
	pulse_clock();
	j = j>>1;
    }
}



void send_braille(char * buff) {
        for (int x = 19; x >= 0; x--) {
//	printf("testing sending x=%0x\n", buff[x]);
	    send_byte(buff[x]);
	}
	do_strobe();
        gpio_set_dir(16, GPIO_OUT);  // debug only 
	gpio_put(16,1);  // debug only
}

void clear_display() {
	for (int x = 0; x<160; x++) {
	    gpio_put(data, x%2);
	    pulse_clock();
	}
	do_strobe();
}

void initialize_braille_display(void) {
    // initialize the i/o pins to out and set them to low.
    gpio_init(clock);
    gpio_set_dir(clock, GPIO_OUT);
    gpio_init(data);
    gpio_set_dir(data, GPIO_OUT);
    gpio_init(strobe);
    gpio_set_dir(strobe, GPIO_OUT);
    gpio_put(clock,0);
    gpio_put(data,0);
    gpio_put(strobe,0);
    clear_display();
}

