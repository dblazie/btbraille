// Copyright (c) 2021 Michael Stoops. All rights reserved.
// Portions copyright (c) 2021 Raspberry Pi (Trading) Ltd.
// 
// Redistribution and use in source and binary forms, with or without modification, are permitted provided that the 
// following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following
//    disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
//    following disclaimer in the documentation and/or other materials provided with the distribution.
// 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote
//    products derived from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE 
// USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// SPDX-License-Identifier: BSD-3-Clause
//
// Example of an SPI bus slave using the PL022 SPI interface

/*   Wiring for SPI5 Interface on CM4 Module to Pico 2040 
		CM4 GPIO #s		Pico 2040 GPIO #s
CS0		12			13
MISO		13			11
MOSI		14			8
CLK		15			14

*/

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"

extern void send_braille(char *);
extern void clear_display(void);
extern void initialize_braille_display();

#define BUF_LEN         50

#define PICO_DEFAULT_SPI_RX_PIN 8
#define PICO_DEFAULT_SPI_SCK_PIN 14
#define PICO_DEFAULT_SPI_TX_PIN 11
#define PICO_DEFAULT_SPI_CSN_PIN 13

void printbuf(uint8_t buf[], size_t len) {
    size_t i;
    for (i = 0; i < 6; ++i) {
        if (i % 16 == 15)
            printf("%02x\n", buf[i]);
        else
            printf("%02x ", buf[i]);
    }

    // append trailing newline if there isn't one
    if (i % 16) {
        putchar('\n');
    }
}

void process_buffer(char *in_buf) {
    uint8_t out_buf[BUF_LEN];
    printf("command %c\n", in_buf[0]);
    switch (in_buf[0]) {
	case 'B': send_braille(in_buf);
		  return;
        case 'C': clear_display();
		  printf("display cleared\n");
		  return;
        case 'T': 
		  printf("test displayed\n");
	    // Initialize output buffer
    		  for (size_t i = 0; i < BUF_LEN; ++i) {
       	    // bit-inverted from i. The values should be: {0xff, 0xfe, 0xfd...}
                      out_buf[i] = ~i;
                  }

        	  send_braille(out_buf);
		  return;
        default: printf("default error, didn't receive a T, C or B for testing only. dbb\n");
                 return;
    }
}

int init_spi() {
    // Enable UART so we can print
    stdio_init_all();
    initialize_braille_display();
    clear_display();  // clear the braille display
#if !defined(spi_default) || !defined(PICO_DEFAULT_SPI_SCK_PIN) || !defined(PICO_DEFAULT_SPI_TX_PIN) || !defined(PICO_DEFAULT_SPI_RX_PIN) || !defined(PICO_DEFAULT_SPI_CSN_PIN)
#warning spi/spi_slave example requires a board with SPI pins
    puts("Default SPI pins were not defined");
#else

    // Enable SPI 1 at 1 MHz and connect to GPIOs
    spi_init(spi1, 1000 * 1000);
    spi_set_slave(spi1, true);
    spi_set_format(spi1, 8, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);
    gpio_set_function(PICO_DEFAULT_SPI_RX_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_CSN_PIN, GPIO_FUNC_SPI);
    // Make the SPI pins available to picotool
    bi_decl(bi_4pins_with_func(PICO_DEFAULT_SPI_RX_PIN, PICO_DEFAULT_SPI_TX_PIN, PICO_DEFAULT_SPI_SCK_PIN, PICO_DEFAULT_SPI_CSN_PIN, GPIO_FUNC_SPI));
    printf("SPI initialized\n");
    return 0;
}

void monitor_spi() {
    uint8_t out_buf[BUF_LEN], in_buf[BUF_LEN];

    for (size_t i = 0; ; ++i) {
        // Write the output buffer to MISO, and at the same time read from MOSI.
//        spi_write_read_blocking(spi_default, out_buf, in_buf, BUF_LEN);
	  while(!spi_is_readable(spi1)){};
          spi_read_blocking(spi1, 0,  in_buf, BUF_LEN);
	  process_buffer((char *) in_buf);
	  while(!spi_is_writable(spi1)){};  // either this while delay or the sleep_ms(1) are needed to get a good xfer.
	  sleep_ms(1);
	  spi_write_blocking(spi1, out_buf, BUF_LEN);
        // Write to stdio whatever came in on the MOSI line.
//          printf("SPI slave says: read page %d from the MOSI line:\n", i);
          printbuf(in_buf, BUF_LEN);
//	  send_braille((char *) in_buf);
    }
#endif
}
