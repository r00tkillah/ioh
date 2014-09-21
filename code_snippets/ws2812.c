#include <ws2812.h>
#include <stdbool.h>
#include <stdint.h>
#include <jendefs.h>

/* Hardware includes */
#include <AppHardwareApi.h>
#include <PeripheralRegs.h>

/*
 * based on information from
 * http://cpldcpu.wordpress.com/2014/01/14/light_ws2812-library-v2-0-part-i-understanding-the-ws2812/
 *
 * Both 0 and 1 are encoded as pulses
 * 1000 encodes a 0, with a pulse of 500 ns
 * 1110 encodes a 1, with a pulse of 1500 ns
 *
 * The ws2812 is a 5v part, and we use a transistor hooked to a pullup,
 * so that effectively inverts our signal.
 *
 * we're inverted so it's 0111 and 0001
 */
#define ZEROPULSE 0x7
#define ONEPULSE  0x1

static uint32_t encode_byte(const uint8_t input)
{
	uint32_t ret = 0;
	uint8_t i;
	for (i = 0; i < 8; i++) {
		uint32_t bit;

		if (input & (1<<i)) {
			bit = ONEPULSE;
		} else {
			bit = ZEROPULSE;
		}
		ret |= bit << (i*4);
	}
	return ret;
}

static void write_pixel(ws_pixel *pixel)
{
	/* pixels are grb with msb first */
	vAHI_SpiStartTransfer(31, encode_byte(pixel->g));
	vAHI_SpiStartTransfer(31, encode_byte(pixel->r));
	vAHI_SpiStartTransfer(31, encode_byte(pixel->b));

	/* resets are said to be 50us, but in practice need to be just more than 9 */
	vAHI_SpiStartTransfer(31, 0); /* 16uS */
}

void ws2812_setup()
{
	/* 2 mhz clock rate (16/(2*4))
	 *
	 * a bit is encoded with 4 bits, making a 500ns slice
	 * and a 2000ns bit cycle
	 * slave enable seems useless, since we just use data out
	 */
	vAHI_SpiConfigure(
			0     /* u8SlaveEnable    */,
			false /* bLsbFirst        */,
			false /* bPolarity        */,
			false /* bPhase           */,
			4     /* u8ClockDivider   */,
			false /* bInterruptEnable */,
			false /* bAutoSlaveSelect */);
}

void ws2812_write(ws_pixel* pixels, uint8_t count)
{
	uint8_t i;

	if (!pixels || !count) {
		return;
	}

	for (i = 0; i < count; i++) {
		write_pixel(pixels+i);
	}
}
