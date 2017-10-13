#ifndef UKECCAK256_H_
#define UKECCAK256_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

// TODO this is copyied, need to share header
typedef struct
{
    union
    {
        uint8_t b[200];
        uint64_t q[25];
    } st;
    size_t offset;
    size_t rate;
    uint8_t delim;
} ukeccak256_ctx;

int ukeccak256(uint8_t* in, size_t inlen, uint8_t* out, size_t outlen);
void ukeccak256_init(ukeccak256_ctx* ctx);
void ukeccak256_deinit(ukeccak256_ctx* ctx);
void ukeccak256_update(ukeccak256_ctx* ctx, uint8_t* in, size_t len);
void ukeccak256_digest(ukeccak256_ctx* ctx, uint8_t* out);
void ukeccak256_finish(ukeccak256_ctx* ctx, uint8_t* out);

#ifdef __cplusplus
}
#endif
#endif
