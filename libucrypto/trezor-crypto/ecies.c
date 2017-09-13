#include "ecies.h"

// 0x04 + echd-random-pubk + iv + aes(kdf(shared-secret), plaintext) + hmac
// * offset 0                65         81               275
// *        [ecies-pubkey:65||aes-iv:16||cipher-text:194||ecies-mac:32]
// *                        ||-----------hmac-----------||

int
ucrypto_ecies_encrypt_str(ucrypto_ecp_point* p,
                          const uint8_t* s,
                          size_t slen,
                          int radix,
                          const char* plain,
                          uint8_t* cipher)
{
    return 0;
}

int
ucrypto_ecies_encrypt_mpi(ucrypto_ecp_point* p,
                          const uint8_t* s,
                          size_t slen,
                          ucrypto_mpi* bin,
                          uint8_t* cipher)
{
    return 0;
}

int
ucrypto_ecies_encrypt(ucrypto_ecp_point* p,
                      const uint8_t* s,
                      size_t slen,
                      const uint8_t* in,
                      size_t inlen,
                      uint8_t* out)
{
    return 0;
}

int
ucrypto_ecies_decrypt_str(ucrypto_ecc_ctx* s,
                          const uint8_t* smac,
                          size_t smaclen,
                          int radix,
                          const char* cipher,
                          uint8_t* plain)
{
    return 0;
}

int
ucrypto_ecies_decrypt_mpi(ucrypto_ecc_ctx* s,
                          const uint8_t* smac,
                          size_t smaclen,
                          ucrypto_mpi* bin,
                          uint8_t* plain)
{
    return 0;
}

int
ucrypto_ecies_decrypt(ucrypto_ecc_ctx* secret,
                      const uint8_t* smac,
                      size_t smaclen,
                      const uint8_t* cipher,
                      size_t len,
                      uint8_t* plain)
{
    return 0;
}

int
ucrypto_ecies_kdf_str(const char* str, int radix, uint8_t* b, size_t keylen)
{
    return 0;
}

int
ucrypto_ecies_kdf_mpi(const ucrypto_mpi* secret, uint8_t* b, size_t keylen)
{
    return 0;
}

void
ucrypto_ecies_kdf(uint8_t* z, size_t zlen, uint8_t* b, size_t keylen)
{
}

//
//
//
