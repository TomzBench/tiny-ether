#include "uecies_decrypt.h"

// 0x04 + echd-random-pubk + iv + aes(kdf(shared-secret), plaintext) + hmac
// * offset 0                65         81               275
// *        [ecies-pubkey:65||aes-iv:16||cipher-text:194||ecies-mac:32]
// *                        ||-----------hmac-----------||

int
uecies_decrypt(uecc_ctx* ctx,
               const uint8_t* shared_mac,
               size_t shared_mac_len,
               const uint8_t* cipher,
               size_t len,
               uint8_t* plain)
{
    int sz;
    uint8_t key[32];  // kdf(ecdh_agree(secret,ecies-pubkey));
    uint8_t mkey[32]; // sha256(key[16]);
    uint8_t tmac[32]; // hmac_sha256(iv+cipher+shared_mac)
    uaes_iv_128* iv = (uaes_iv_128*)&cipher[65];
    uaes_ctr_128_key* ekey = (uaes_ctr_128_key*)key;
    uhmac_sha256_ctx hmac;

    sz = uecc_agree_bin(ctx, cipher, 65);
    if (sz) return -1;

    uhash_kdf(&ctx->z.b[1], 32, key, 32);
    usha256(&key[16], 16, mkey);
    uhmac_sha256_init(&hmac, mkey, 32);
    uhmac_sha256_update(&hmac, &cipher[65], len - 32 - 65);
    uhmac_sha256_update(&hmac, shared_mac, shared_mac_len);
    uhmac_sha256_finish(&hmac, tmac);
    uhmac_sha256_free(&hmac);
    for (size_t i = 0; i < 32; i++) {
        if (!(tmac[i] == cipher[len - 32 + i])) return -1;
    }

    sz = uaes_crypt_ctr_128(ekey, iv, &cipher[81], len - 32 - 16 - 65, plain);

    return sz ? sz : len - 32 - 16 - 65;
}
