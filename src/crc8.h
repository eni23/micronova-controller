/**@brief CRC-8 routine
 * @author Richard James Howe
 * @license MIT */
#ifndef CRC8_H
#define CRC8_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

uint8_t crc8(const uint8_t * const data, const size_t length);

#ifdef __cplusplus
}
#endif
#endif
