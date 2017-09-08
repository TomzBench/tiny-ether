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

#include "dh.h"

int ucrypto_ecies_decrypt(ucrypto_ecdh_ctx* s,
                          uint8_t* cipher,
                          size_t cipher_len,
                          uint8_t* plain,
                          size_t plain_len);

#ifdef __cplusplus
}
#endif
#endif
