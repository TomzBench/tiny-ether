#ifndef ECIES_H_
#define ECIES_H_

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief
 *    ECIES Encrypt, where P = recipient public key is:
 *    1) generate r = random value
 *    2) generate shared-secret = kdf( ecdhAgree(r, P) )
 *    3) generate R = rG [same op as generating a public key]
 *    4) send 0x04 || R || AsymmetricEncrypt(shared-secret, plaintext) || tag
 *    currently used by go:
 *    ECIES_AES128_SHA256 = &ECIESParams{
 *        Hash: sha256.New,
 *        hashAlgo: crypto.SHA256,
 *        Cipher: aes.NewCipher,
 *        BlockSize: aes.BlockSize,
 *        KeyLen: 16,
 *        }
 *
 *	let mut msg = vec![0u8; (1 + 64 + 16 + plain.len() + 32)];
 *	0x04 + echd-random-pubk + iv + aes(kdf(shared-secret), plaintext) + hmac
 *
 * offset 0                65         81               275
 *        [ecies-pubkey:65||aes-iv:16||cipher-text:194||ecies-mac:32]
 */

#include "mbedtls/sha256.h"
#include "uaes.h"
#include "uecc.h"
#include "uhmac.h"
#include <string.h>

int uecies_encrypt_str(uecc_point*,
                       const uint8_t* smac,
                       size_t smaclen,
                       int radix,
                       const char* plain,
                       uint8_t* cipher);

int uecies_encrypt_mpi(uecc_point*,
                       const uint8_t* smac,
                       size_t smaclen,
                       ubn* bin,
                       uint8_t* cipher);

int uecies_encrypt(uecc_point*,
                   const uint8_t* smac,
                   size_t smaclen,
                   const uint8_t* plain,
                   size_t plain_len,
                   uint8_t* cipher);

int uecies_decrypt_str(uecc_ctx* s,
                       const uint8_t*,
                       size_t,
                       int radix,
                       const char* cipher,
                       uint8_t* plain);

int uecies_decrypt_mpi(uecc_ctx* s,
                       const uint8_t*,
                       size_t,
                       ubn* bin,
                       uint8_t* plain);

int uecies_decrypt(uecc_ctx* s,
                   const uint8_t*,
                   size_t,
                   const uint8_t* cipher,
                   size_t cipher_len,
                   uint8_t* plain);

int uecies_kdf_str(const char* str, int radix, uint8_t* b, size_t keylen);
int uecies_kdf_mpi(const ubn* secret, uint8_t* b, size_t keylen);
void uecies_kdf(uint8_t* z, size_t zlen, uint8_t* key, size_t keylen);

#ifdef __cplusplus
}
#endif
#endif
