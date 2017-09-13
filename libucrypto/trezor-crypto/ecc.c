#include "ecc.h"

// clang-format off
#define IF_ERR_EXIT(f)                    \
    do {                                  \
        if ((err = (f)) != 0) goto EXIT;  \
    } while (0)
#define IF_NEG_EXIT(val, f)               \
    do {                                  \
        if ((val = (f)) < 0) goto EXIT;   \
    } while (0)
// clang-format off

int
ucrypto_ecc_key_init(ucrypto_ecc_ctx* ctx, const ucrypto_mpi* d)
{
	return 0;
}

int
ucrypto_ecc_key_init_string(ucrypto_ecc_ctx* ctx, int radix, const char* s)
{
	return 0;
}

int
ucrypto_ecc_key_init_binary(ucrypto_ecc_ctx* ctx, const ucrypto_mpi* d)
{
	return 0;
}

int
ucrypto_ecc_key_init_new(ucrypto_ecc_ctx* ctx)
{
	return 0;
}

void
ucrypto_ecc_key_deinit(ucrypto_ecc_ctx* ctx)
{
}

int
ucrypto_ecc_atop(const char* str, int rdx, ucrypto_ecp_point* q)
{
	return 0;
}

int
ucrypto_ecc_ptob(ucrypto_ecp_point* p, ucrypto_ecc_public_key* b)
{
	return 0;
}

int
ucrypto_ecc_btop(ucrypto_ecc_public_key* k, ucrypto_ecp_point* p)
{
	return 0;
}

int
ucrypto_ecc_point_copy(const ucrypto_ecp_point* src, ucrypto_ecp_point* dst)
{
	return 0;
}

int
ucrypto_ecc_agree(ucrypto_ecc_ctx* ctx, const ucrypto_ecc_public_key* key)
{
	return 0;
}

int
ucrypto_ecc_agree_point(ucrypto_ecc_ctx* ctx, const ucrypto_ecp_point* qp)
{
	return 0;
}

int
ucrypto_ecc_sign(ucrypto_ecc_ctx* ctx,
                 const uint8_t* b,
                 uint32_t sz,
                 ucrypto_ecc_signature* sig_p)
{
	return 0;
}

int
ucrypto_ecc_verify(const ucrypto_ecp_point* q,
                   const uint8_t* b,
                   uint32_t sz,
                   ucrypto_ecc_signature* sig_p)
{
	return 0;
}
int
ucrypto_ecc_recover(const ucrypto_ecc_signature* sig,
                    const uint8_t* digest,
                    int recid,
                    ucrypto_ecc_public_key* key)
{
	return 0;
}

//
//
//
