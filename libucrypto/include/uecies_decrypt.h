#ifndef UECIES_DECRYPT_H_
#define UECIES_DECRYPT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "uaes.h"
#include "uecc.h"
#include "uhash.h"

int uecies_decrypt(uecc_ctx* ctx,
                  const uint8_t* shared_mac,
                  size_t shared_mac_len,
                  const uint8_t* cipher,
                  size_t cipher_len,
                  uint8_t* clear);

#ifdef __cplusplus
}
#endif
#endif
