#include "rlpx_discovery.h"
#include "ukeccak256.h"

// h256:32 + Signature:65 + type + RLP
int
rlpx_disc_parse(usys_sockaddr* addr,
                const uint8_t* b,
                uint32_t l,
                uecc_public_key* node_id,
                int* type,
                urlp** rlp)
{
    // Stack
    h256 hash, shash;
    uecc_shared_secret* s = (uecc_shared_secret*)shash.b;
    int err;

    // Check len before parsing around
    if (l < (sizeof(h256) + 65 + 3)) return -1;

    // Check hash  hash = sha3(sig, type, rlp)
    ukeccak256((uint8_t*)&b[32], l - 32, hash.b, 32);
    if (memcmp(hash.b, b, 32)) return -1;

    // Recover signature from signed hash of type+rlp
    ukeccak256((uint8_t*)&b[32 + 65], l - (32 + 65), shash.b, 32);
    err = uecc_recover_bin(&b[32], s, node_id);

    // Return OK
    *type = b[32 + 65];
    *rlp = urlp_parse(&b[32 + 65 + 1], l - (32 + 65 + 1));
    return 0;
}
