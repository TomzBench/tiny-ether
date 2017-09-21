#ifndef UECIES_ENCRYPT_H_
#define UECIES_ENCRYPT_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "uaes.h"
#include "uecc.h"
#include "uhash.h"

int uecies_encrypt(const uecc_public_key*,
                   const uint8_t* shared_mac,
                   size_t shared_mac_len,
                   const uint8_t* plain,
                   size_t plain_len,
                   uint8_t* cipher);

size_t uecies_encrypt_size(size_t len);

#ifdef __cplusplus
}
#endif
#endif
