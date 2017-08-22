/**
 * @file urlp.c
 *
 * @brief Encode and decode bytes and lists of bytes into rlp format.
 *
 * Summary of encoding scheme
 *
 * [0x00,0x7f] single byte
 * [0x80,0xb7] 0-55 bytes
 * [0xb8,0xbf] 56-2^56 bytes
 * [0xc0,0xf7] 0-55 bytes of concatenated items.
 * [0xf8,0xff] 56-2^56 bytes of concatenated items.
 *
 * Where an "item" is described with first byte < 0xc0 and a "list" is described
 * with first byte >= 0xc0 and is concatenation of items.
 *
 * And the maximum size of an item or list is 2^56 - because the length of
 * length of bytes can be at most 7 bytes in length.
 *
 */

#include "urlp.h"

int urlp_encoder_init(urlp_encoder *enc, uint8_t *bytes, uint32_t size) {
    memset(enc, 0, sizeof(urlp_encoder));
    enc->size = enc->spot = size;
    enc->bytes = bytes;
    return 0;
}

uint32_t urlp_item(urlp_encoder *enc, uint8_t *src, uint32_t slen) {
    uint32_t size;
    if (!enc->spot) return 0;
    if (slen == 0) {
	size = 1;
	enc->bytes[enc->spot--] = 0x80u;
    } else if (slen == 1 && src[0] < 0x80) {
	size = 1;
	enc->bytes[enc->spot--] = src[0];
    } else if (slen <= 55) {
	size = slen + 1;
	if (enc->spot - size < 0) return 0;  // negative result possible?
	enc->spot -= size;
	enc->bytes[enc->spot] = 0x80 + slen;
	memcpy(&enc->bytes[enc->spot + 1], src, slen);
    } else {
	int szsz = 4 - (urlp_clz_fn(slen) / 8);
	size = szsz + 1 + slen;
	uint8_t(*b)[4] = (uint8_t(*)[4])(&slen);
	if (enc->spot - size < 0) return 0;  // negative result possible?
	enc->spot -= size;
	enc->bytes[enc->spot] = 0xb7 + szsz;
	for (int i = 0; i < szsz; i++) {
	    // Portable ?
	    enc->bytes[enc->spot + 1 + i] = *b[i];
	}
	memcpy(&enc->bytes[enc->spot + 1 + szsz], src, slen);
    }
    return size;
}

uint32_t urlp_list(urlp_encoder *enc, int n, ...) {
    uint32_t bytes = urlp_size(enc);
    if (bytes <= 55) {
	enc->bytes[--enc->spot] = 0xc0 + bytes;
    } else {
	// Portable ?
	int szsz = 4 - (urlp_clz_fn(bytes) / 8);
	uint8_t(*b)[4] = (uint8_t(*)[4])(&bytes);
	for (int i = 0; i < szsz; i++) {
	    enc->bytes[enc->spot + 1 + i] = *b[i];
	}
    }
    return bytes;
}

uint32_t urlp_size(urlp_encoder *enc) { return enc->size - enc->spot; }
uint8_t *urlp_data(urlp_encoder *enc) { return &enc->bytes[enc->spot]; }

//
//
//
