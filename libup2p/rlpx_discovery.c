#include "rlpx_discovery.h"
#include "ukeccak256.h"

int
rlpx_discovery_recv(usys_sockaddr* ep, const uint8_t* b, uint32_t l)
{
    uecc_public_key pub;
    RLPX_DISCOVERY type;
    int err = -1;
    urlp* rlp;
    const urlp* crlp;

    // Parse (rlp is allocated on success - must free)
    if ((err = rlpx_discovery_parse(ep, b, l, &pub, (int*)&type, &rlp))) {
        return err;
    }

    crlp = rlp;
    if (type == RLPX_DISCOVERY_PING) {
        err = rlpx_discovery_parse_ping(ep, &crlp);
    } else if (type == RLPX_DISCOVERY_PING) {
        err = rlpx_discovery_parse_pong(ep, &crlp);
    } else if (type == RLPX_DISCOVERY_FIND) {
        err = rlpx_discovery_parse_find(ep, &crlp);
    } else if (type == RLPX_DISCOVERY_NEIGHBOURS) {
        err = rlpx_discovery_parse_neighbours(ep, &crlp);
    } else {
        // error
    }

    // Free and return
    urlp_free(&rlp);
    return err;
}

// h256:32 + Signature:65 + type + RLP
int
rlpx_discovery_parse(usys_sockaddr* addr,
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

int
rlpx_discovery_parse_ping(usys_sockaddr* addr, const urlp** rlp)
{
    // TODO
    return 0;
}

int
rlpx_discovery_parse_pong(usys_sockaddr* addr, const urlp** rlp)
{
    // TODO
    return 0;
}

int
rlpx_discovery_parse_find(usys_sockaddr* addr, const urlp** rlp)
{
    // TODO - send neighbours
    return 0;
}

int
rlpx_discovery_parse_neighbours(usys_sockaddr* addr, const urlp** rlp)
{
    // TODO - populate our node table
    return 0;
}
