#include "test.h"

uint32_t g_disc_ping_v4_len;
uint32_t g_disc_ping_v555_len;
uint32_t g_disc_pong_len;
uint32_t g_disc_find_node_len;
uint32_t g_disc_neighbours_len;

const char* g_disc_ping_v4 = DISCOVERY_PING_V4;
const char* g_disc_ping_v555 = DISCOVERY_PING_V555;
const char* g_disc_pong = DISCOVERY_PONG;
const char* g_disc_find_node = DISCOVERY_FIND_NODE;
const char* g_disc_neighbours = DISCOVERY_NEIGHBOURS;

uint8_t* g_disc_ping_v4_bin = NULL;
uint8_t* g_disc_ping_v555_bin = NULL;
uint8_t* g_disc_pong_bin = NULL;
uint8_t* g_disc_find_node_bin = NULL;
uint8_t* g_disc_neighbours_bin = NULL;

// Encrypt a udp packet
int test_disc_write();

// Decrypt a udp packet
int test_disc_read();

// Test some protocol ops
int test_disc_protocol();

// check functions
int check_ping_v4(int type, urlp* rlp);
int check_ping_v555(int type, urlp* rlp);
int check_pong(int type, urlp* rlp);
int check_find_node(int type, urlp* rlp);
int check_neighbours(int type, urlp* rlp);

int
test_discovery()
{
    int err = 0;

    // Init test vectors.
    g_disc_ping_v4_len = strlen(g_disc_ping_v4) / 2;
    g_disc_ping_v555_len = strlen(g_disc_ping_v555) / 2;
    g_disc_pong_len = strlen(g_disc_pong) / 2;
    g_disc_find_node_len = strlen(g_disc_find_node) / 2;
    g_disc_neighbours_len = strlen(g_disc_neighbours) / 2;
    g_disc_ping_v4_bin = rlpx_malloc(g_disc_ping_v4_len);
    g_disc_ping_v555_bin = rlpx_malloc(g_disc_ping_v555_len);
    g_disc_pong_bin = rlpx_malloc(g_disc_pong_len);
    g_disc_find_node_bin = rlpx_malloc(g_disc_find_node_len);
    g_disc_neighbours_bin = rlpx_malloc(g_disc_neighbours_len);
    memcpy(g_disc_ping_v4_bin, //
           makebin(g_disc_ping_v4, NULL),
           g_disc_ping_v4_len);
    memcpy(g_disc_ping_v555_bin,
           makebin(g_disc_ping_v555, NULL),
           g_disc_ping_v555_len);
    memcpy(g_disc_pong_bin, makebin(g_disc_pong, NULL), g_disc_pong_len);
    memcpy(g_disc_find_node_bin,
           makebin(g_disc_find_node, NULL),
           g_disc_find_node_len);
    memcpy(g_disc_neighbours_bin,
           makebin(g_disc_neighbours, NULL),
           g_disc_neighbours_len);

    err |= test_disc_read();
    err |= test_disc_write();
    err |= test_disc_protocol();

    // Free test vectors
    rlpx_free(g_disc_ping_v4_bin);
    rlpx_free(g_disc_ping_v555_bin);
    rlpx_free(g_disc_pong_bin);
    rlpx_free(g_disc_find_node_bin);
    rlpx_free(g_disc_neighbours_bin);
    return err;
}

int
test_disc_write()
{
    return 0;
}

int
test_disc_read()
{
    urlp* rlp = NULL;
    uecc_public_key nodeid;
    int type, err;
    usys_sockaddr sock_addr;

    // Construct test vector arrays for loop
    const uint8_t* reads[5] = { g_disc_ping_v4_bin,
                                g_disc_ping_v555_bin,
                                g_disc_pong_bin,
                                g_disc_find_node_bin,
                                g_disc_neighbours_bin };
    uint32_t reads_sz[5] = { g_disc_ping_v4_len,
                             g_disc_ping_v555_len,
                             g_disc_pong_len,
                             g_disc_find_node_len,
                             g_disc_neighbours_len };
    int (*check_fn[5])(int, urlp*) = { check_ping_v4,
                                       check_ping_v555,
                                       check_pong,
                                       check_find_node,
                                       check_neighbours };

    for (int i = 0; i < 5; i++) {
        err = rlpx_disc_parse(
            &sock_addr, reads[i], reads_sz[i], &nodeid, &type, &rlp);
        if (!err) {
            err = check_fn[i](type, rlp);
            urlp_free(&rlp);
        }
    }

    return 0;
}

int
test_disc_protocol()
{
    return 0;
}

int
check_ping_v4(int type, urlp* rlp)
{

    int err = -1;
    int ver = urlp_as_u32(urlp_at(rlp, 0));
    if (type != 1) return err;
    if (!(ver == 4)) return err;
    err = 0;
    return err;
}

int
check_ping_v555(int type, urlp* rlp)
{
    int err = -1;
    int ver = urlp_as_u32(urlp_at(rlp, 0));
    if (type != 1) return err;
    if (!(ver == 555)) return err;
    err = 0;
    return err;
}

int
check_pong(int type, urlp* rlp)
{
    int err = -1;
    if (type != 2) return err;
    err = 0;
    return err;
}

int
check_find_node(int type, urlp* rlp)
{
    int err = -1;
    if (type != 3) return err;
    err = 0;
    return err;
}

int
check_neighbours(int type, urlp* rlp)
{
    int err = -1;
    if (type != 4) return err;
    err = 0;
    return err;
}
