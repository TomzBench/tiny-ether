#include "ecies.h"

int
ucrypto_ecies_decrypt(ucrypto_ecdh_ctx* secret,
                      uint8_t* cipher,
                      size_t cipher_len,
                      uint8_t* plain,
                      size_t plain_len)
{
    // TODO: Encryption
    // 0x04 + echd-random-pubk + iv + aes(kdf(shared-secret), plaintext) + hmac
    // * offset 0                65         81               275
    // *        [ecies-pubkey:65||aes-iv:16||cipher-text:194||ecies-mac:32]
    int err = 0;
    char s[800];
    size_t slen = 800;
    err = ucrypto_ecdh_agree(secret, (ucrypto_ecdh_public_key*)cipher);
    ucrypto_mpi_write_string(ucrypto_ecdh_secret(secret), 16, s, slen, &slen);
    printf("%s\n\n", s);
    ((void)secret);
    ((void)cipher_len);
    ((void)plain);
    ((void)plain_len);
    return err;
}
