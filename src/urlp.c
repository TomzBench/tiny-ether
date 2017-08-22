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

uint32_t urlp_print_long_size(urlp_encoder *enc, uint32_t size, uint8_t pref);

int urlp_encoder_init(urlp_encoder *enc, uint8_t *bytes, uint32_t size) {
    memset(enc, 0, sizeof(urlp_encoder));
    enc->size = enc->spot = size;  // start out of range for writer.
    enc->bytes = bytes;
    return 0;
}

uint32_t urlp_item(urlp_encoder *enc, uint8_t *src, uint32_t slen) {
    uint32_t size;
    if (!enc->spot) return 0;
    if (slen == 0) {
	size = 1;
	enc->bytes[--enc->spot] = 0x80u;
    } else if (slen == 1 && src[0] < 0x80) {
	size = 1;
	enc->bytes[--enc->spot] = src[0];
    } else if (slen <= 55) {
	size = slen + 1;
	if (enc->spot - size < 0) return 0;  // negative result possible?
	for (int i = slen; i; i--) enc->bytes[--enc->spot] = src[i - 1];
	enc->bytes[--enc->spot] = 0x80 + slen;
    } else {
	for (int i = slen; i; i--) enc->bytes[--enc->spot] = src[i - 1];
	size = urlp_print_long_size(enc, slen, 0xb7) + slen + 1;
    }
    return size;
}

uint32_t urlp_list(urlp_encoder *enc, int n, ...) {
    uint32_t bytes = urlp_size(enc);
    if (bytes <= 55) {
	enc->bytes[--enc->spot] = 0xc0 + bytes++;
    } else {
	bytes += urlp_print_long_size(enc, bytes, 0xc0) + 1;
    }
    return bytes;
}

uint32_t urlp_print_long_size(urlp_encoder *enc, uint32_t size, uint8_t pref) {
    uint32_t szsz = 4 - (urlp_clz_fn(size) / 8);
    uint8_t(*b)[4] = (uint8_t(*)[4])(&size);
    for (int i = 0; i < szsz; i++) enc->bytes[--enc->spot] = *b[i];
    enc->bytes[--enc->spot] = pref + szsz;
    return szsz;
}

uint32_t urlp_size(urlp_encoder *enc) { return enc->size - enc->spot; }
uint8_t *urlp_data(urlp_encoder *enc) { return &enc->bytes[enc->spot]; }

//
//
//
