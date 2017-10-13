#ifndef RLPX_HELPER_MACROS_H_
#define RLPX_HELPER_MACROS_H_

// Static assertion helper.
#define ASSERT_CONCAT_(a, b) a##b
#define ASSERT_CONCAT(a, b) ASSERT_CONCAT_(a, b)
#define ct_assert(e) enum { ASSERT_CONCAT(assert_line, __LINE__) = 1 / (!!(e)) }

#define AES_LEN(l) ((l) % 16 ? ((l) + 16 - ((l) % 16)) : (l))

#define READ_BE(l, dst, src)                                                   \
    do {                                                                       \
        uint32_t be = 1;                                                       \
        if (*(char*)&be == 0) {                                                \
            for (int i = 0; i < l; i++) ((uint8_t*)dst)[i] = src[i];           \
        } else {                                                               \
            for (int i = 0; i < l; i++) ((uint8_t*)dst)[(l)-1 - i] = src[i];   \
        }                                                                      \
    } while (0)

#define WRITE_BE(l, dst, src)                                                  \
    do {                                                                       \
        uint32_t be = 1, hit = 0;                                              \
        if (*(char*)&be == 0) {                                                \
            for (int i = 0; i < l; i++) {                                      \
                if ((src)[i]) hit = 1;                                         \
                if (hit || (src)[i] || !i) (dst)[i] = (src)[i];                \
            }                                                                  \
        } else {                                                               \
            for (int i = 0; i < l; i++) {                                      \
                if ((src)[i]) hit = 1;                                         \
                if (hit || (src)[i] || !i) (dst)[(l)-1 - i] = (src)[i];        \
            }                                                                  \
        }                                                                      \
    } while (0)

#define XORN(x, b, l)                                                          \
    do {                                                                       \
        for (int i = 0; i < l; i++) x[i] ^= b[i];                              \
    } while (0)

#define XOR32(x, b)                                                            \
    do {                                                                       \
        for (int i = 0; i < 32; i++) x[i] ^= b[i];                             \
    } while (0)

#define XOR32_SET(y, x, b)                                                     \
    do {                                                                       \
        for (int i = 0; i < 32; i++) y[i] = x[i] ^ b[i];                       \
    } while (0)

#endif
