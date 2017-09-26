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

typedef struct
{
    rlpx *alice, *bob;           /*!< rlpx context */
    size_t authlen, acklen;      /*!< size of cipher */
    uint8_t auth[800], ack[800]; /*!< cipher test buffers*/
} test_session;

const uint8_t* makebin(const char* str, size_t* len);
int check_q(const uecc_public_key* key, const char* str);
int test_session_init(test_session*, int);
void test_session_deinit(test_session*);

int test_handshake();
int test_frame();

#endif
