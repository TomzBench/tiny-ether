#include "rlpx_internal.h"

void
rlpx_test_nonce_set(rlpx* s, h256* nonce)
{
    memcpy(s->nonce.b, nonce->b, 32);
}

void
rlpx_test_remote_nonce_set(rlpx* s, h256* nonce)
{
    memcpy(s->remote_nonce.b, nonce->b, 32);
}
