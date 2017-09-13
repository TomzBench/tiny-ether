#ifndef HASH_H_
#define HASH_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "mpi.h"

typedef struct
{
    uint8_t b[256];
} b2048;

typedef struct
{
    uint8_t b[128];
} b1024;

typedef struct
{
    uint8_t b[65];
} b520;

typedef struct
{
    uint8_t b[64];
} b512;

typedef struct
{
    uint8_t b[32];
} b256;

typedef struct
{
    uint8_t b[20];
} b160;

typedef struct
{
    uint8_t b[16];
} b128;

typedef struct
{
    uint8_t b[8];
} b64;

#ifdef __cplusplus
}
#endif
#endif
