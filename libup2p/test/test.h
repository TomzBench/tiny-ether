#ifndef TEST_H_
#define TEST_H_

#include "rlpx.h"
#include "test_vectors.h"
#include "unonce.h"
#include <string.h>

// clang-format off
#define IF_ERR_EXIT(f)                    \
    do {                                  \
        if ((err = (f)) != 0) goto EXIT;  \
    } while (0)
#define IF_NEG_EXIT(val, f)               \
    do {                                  \
        if ((val = (f)) < 0) goto EXIT;   \
    } while (0)
// clang-format on

typedef struct
{
    const char* auth;    /*!< auth cipher text */
    const char* ack;     /*!< ack cipher text */
    const char* alice_s; /*!< static key */
    const char* alice_e; /*!< ephermeral key */
    const char* alice_n; /*!< nonce */
    const char* bob_s;   /*!< static key */
    const char* bob_e;   /*!< ephermeral key */
    const char* bob_n;   /*!< nonce */
    uint64_t authver;
    uint64_t ackver;
} test_vector;

typedef struct
{
    rlpx_channel alice, bob;     /*!< rlpx context */
    size_t authlen, acklen;      /*!< size of cipher */
    uint8_t auth[800], ack[800]; /*!< cipher test buffers */
    h256 alice_n, bob_n;         /*!< nonces used sometimes */
} test_session;

const uint8_t* makebin(const char* str, size_t* len);
int cmp_q(const uecc_public_key* a, const uecc_public_key* b);
int check_q(const uecc_public_key* key, const char* str);
int test_session_init(test_session*, int);
void test_session_deinit(test_session*);

int test_handshake();
int test_frame();

#endif
