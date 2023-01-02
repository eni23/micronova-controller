/* 8-bit CRC for polynomial */
#include <assert.h>
#include "crc8.h"
#define POLYVAL (0xEBu) /**< https://users.ece.cmu.edu/~koopman/crc/ */
#define INIT    (0xFFu)

static inline uint8_t crc8_core(uint8_t data, uint8_t crc) {
	crc ^= data;
	for (int i = 0; i < 8; i++) {
		const int msb = !!(crc & 0x80);
		crc <<= 1u;
		if (msb)
			crc ^= POLYVAL;
	}
	return crc;
}

uint8_t crc8(const uint8_t * const data, const size_t length) {
	assert(data);
	uint8_t crc = INIT;
	for (size_t i = 0; i < length; i++)
		crc = crc8_core(data[i], crc);
	return crc;
}
