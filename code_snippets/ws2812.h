#ifndef WS2812_H
#define WS2812_H

/*
 * This code compiles.  It could possibly work, too.
 */

#include <stdint.h>

typedef struct __attribute__ ((__packed__)) ws_pixel {
	uint8_t r;
	uint8_t g;
	uint8_t b;
}ws_pixel;

/**
 * Call this before ws2812_write, obviously
 */
void ws2812_setup();

/**
 * pixels is ptr to array of ws_pixels
 * count is the number of pixels
 */
void ws2812_write(ws_pixel* pixels, uint8_t count);

#endif
