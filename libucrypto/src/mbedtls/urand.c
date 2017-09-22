// urand.c

#include "urand.h"

int urand(uint8_t* b, size_t l);
int urand_w_custom(uint8_t* b, size_t l, const uint8_t* pers, size_t psz);
int urand_min_max_u8(uint8_t, uint8_t);

int
urand(uint8_t* b, size_t l)
{
    return urand_w_custom(b, l, NULL, 0);
}

int
urand_w_custom(uint8_t* b, size_t l, const uint8_t* pers, size_t psz)
{
    int err;
    mbedtls_ctr_drbg_context ctx;
    mbedtls_entropy_context ent;

    mbedtls_ctr_drbg_init(&ctx);
    mbedtls_entropy_init(&ent);

    err = mbedtls_ctr_drbg_seed(&ctx, mbedtls_entropy_func, &ent, pers, psz);
    if (!err) err = mbedtls_ctr_drbg_random(&ctx, b, l);

    mbedtls_ctr_drbg_free(&ctx);
    mbedtls_entropy_free(&ent);
    return err;
}

// int padAmount(rand()%100 + 100);
int
urand_min_max_u8(uint8_t start, uint8_t end)
{
    uint8_t r;
    urand(&r, 1);
    if (end <= start) return -1;
    return r % (end - start) + start;
}

//
//
//
