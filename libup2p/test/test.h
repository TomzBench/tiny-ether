#ifndef TEST_H_
#define TEST_H_

#include "rlpx.h"
#include "test_vectors.h"
#include <string.h>

typedef struct
{
    const char* auth;
    const char* ack;
    uint64_t authver;
    uint64_t ackver;
} test_vector;

const uint8_t* makebin(const char* str, size_t* len);
int check_q(const uecc_public_key* key, const char* str);

int test_handshake();
int test_frame();

#endif
