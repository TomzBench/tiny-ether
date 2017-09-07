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
 *	0x04 + echd-random-pubk + iv + aes(shared-secret, plaintext) + hmac
 */

#include "dh.h"

#ifdef __cplusplus
}
#endif
#endif
