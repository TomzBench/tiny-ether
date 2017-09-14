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
// clang-format on

int
uecc_key_init(uecc_ctx* ctx, const ubn* d)
{
    return d ? uecc_key_init_binary(ctx, d) : uecc_key_init_new(ctx);
}

int
uecc_key_init_string(uecc_ctx* ctx, int radix, const char* s)
{
    int err = 0;
    ubn d;
    IF_ERR_EXIT(ubn_str(&d, radix, s));
    IF_ERR_EXIT(uecc_key_init_binary(ctx, &d));
EXIT:
    return err;
}

int
uecc_key_init_binary(uecc_ctx* ctx, const ubn* d)
{
    return 0;
}

int
uecc_key_init_new(uecc_ctx* ctx)
{
    return 0;
}

void
uecc_key_deinit(uecc_ctx* ctx)
{
}

int
uecc_atop(const char* str, int rdx, uecp_point* q)
{
    return 0;
}

int
uecc_ptob(uecp_point* p, uecc_public_key* b)
{
    return 0;
}

int
uecc_btop(uecc_public_key* k, uecp_point* p)
{
    return 0;
}

int
uecc_point_copy(const uecp_point* src, uecp_point* dst)
{
    return 0;
}

int
uecc_agree(uecc_ctx* ctx, const uecc_public_key* key)
{
    return 0;
}

int
uecc_agree_point(uecc_ctx* ctx, const uecp_point* qp)
{
    return 0;
}

int
uecc_sign(uecc_ctx* ctx, const uint8_t* b, uint32_t sz, uecc_signature* sig_p)
{
    return 0;
}

int
uecc_verify(const uecp_point* q,
            const uint8_t* b,
            uint32_t sz,
            uecc_signature* sig_p)
{
    return 0;
}
int
uecc_recover(const uecc_signature* sig,
             const uint8_t* digest,
             int recid,
             uecc_public_key* key)
{
    return 0;
}

//
//
//
