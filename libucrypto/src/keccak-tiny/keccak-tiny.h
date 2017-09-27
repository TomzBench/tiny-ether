#ifndef KECCAK_FIPS202_H
#define KECCAK_FIPS202_H
#define __STDC_WANT_LIB_EXT1__ 1
#include <stdint.h>
#include <stdlib.h>

typedef struct{
	uint8_t a[25*8];
	size_t offset;
	size_t rate;
	uint8_t delim;
}ukeccak256_ctx;

#define decshake(bits) \
  int shake##bits(uint8_t*, size_t, const uint8_t*, size_t);

#define decsha3(bits) \
  int sha3_##bits(uint8_t*, size_t, const uint8_t*, size_t);

#define deckeccak(bits) \
  int keccak_##bits(uint8_t*, size_t, const uint8_t*, size_t);

decshake(128)
decshake(256)
decsha3(224)
decsha3(256)
decsha3(384)
decsha3(512)

deckeccak(256)

void ukeccak256_init(ukeccak256_ctx*);
void ukeccak256_deinit(ukeccak256_ctx*);
void ukeccak256_update(ukeccak256_ctx*, uint8_t *in, size_t l);
void ukeccak256_finish(ukeccak256_ctx*, uint8_t *out);

int ukeccak256(uint8_t* in, size_t inlen, uint8_t* out, size_t outlen);

#endif
