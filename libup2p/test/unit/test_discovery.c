// Copyright 2017 Altronix Corp.
// This file is part of the tiny-ether library
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

/**
 * @author Thomas Chiantia <thomas@altronix>
 * @date 2017
 */

#include "test.h"
#include "urand.h"

uint32_t g_ping_v4_sz;
uint32_t g_ping_v5_sz;
uint32_t g_pong_sz;
uint32_t g_find_node_sz;
uint32_t g_peers_sz;

const char* g_ping_v4_str = DISCOVERY_PING_V4;
const char* g_ping_v5_str = DISCOVERY_PING_V555;
const char* g_pong_str = DISCOVERY_PONG;
const char* g_find_node_str = DISCOVERY_FIND_NODE;
const char* g_peers_str = DISCOVERY_NEIGHBOURS;

uint8_t* g_ping_v4 = NULL;
uint8_t* g_ping_v5 = NULL;
uint8_t* g_pong = NULL;
uint8_t* g_find = NULL;
uint8_t* g_peers = NULL;

// Encrypt a udp packet
int test_disc_write();

// Decrypt a udp packet
int test_disc_read();

// Test some protocol ops
int test_disc_protocol();

// check functions
typedef int (*check_fn)(ktable*, int, const urlp*);
int check_ping_v4(ktable* t, int type, const urlp* rlp);
int check_ping_v5(ktable* t, int type, const urlp* rlp);
int check_pong(ktable* t, int type, const urlp* rlp);
int check_find_node(ktable* t, int type, const urlp* rlp);
int check_neighbours(ktable* t, int type, const urlp* rlp);
void check_neighbours_walk_fn(const urlp*, int idx, void*);

int
test_discovery()
{
    int err = 0;

    // Init test vectors.
    g_ping_v4_sz = strlen(g_ping_v4_str) / 2;
    g_ping_v5_sz = strlen(g_ping_v5_str) / 2;
    g_pong_sz = strlen(g_pong_str) / 2;
    g_find_node_sz = strlen(g_find_node_str) / 2;
    g_peers_sz = strlen(g_peers_str) / 2;
    g_ping_v4 = rlpx_malloc(g_ping_v4_sz);
    g_ping_v5 = rlpx_malloc(g_ping_v5_sz);
    g_pong = rlpx_malloc(g_pong_sz);
    g_find = rlpx_malloc(g_find_node_sz);
    g_peers = rlpx_malloc(g_peers_sz);
    memcpy(g_ping_v4, makebin(g_ping_v4_str, NULL), g_ping_v4_sz);
    memcpy(g_ping_v5, makebin(g_ping_v5_str, NULL), g_ping_v5_sz);
    memcpy(g_pong, makebin(g_pong_str, NULL), g_pong_sz);
    memcpy(g_find, makebin(g_find_node_str, NULL), g_find_node_sz);
    memcpy(g_peers, makebin(g_peers_str, NULL), g_peers_sz);

    err |= test_disc_read();
    err |= test_disc_write();
    err |= test_disc_protocol();

    // Free test vectors
    rlpx_free(g_ping_v4);
    rlpx_free(g_ping_v5);
    rlpx_free(g_pong);
    rlpx_free(g_find);
    rlpx_free(g_peers);
    return err;
}

int
test_disc_write()
{
    // Stack
    int err = -1, type;
    uint32_t l;
    uint8_t b[1000];
    h256 tmp;
    urlp* rlp = NULL;
    uecc_ctx skey;
    uecc_public_key q;
    knodes src, dst;

    // setup test
    // knode_v4_init(&src, NULL, 33342, 123, 456);
    // knode_v4_init(&dst, NULL, 33342, 123, 456);
    // TODO
    uecc_key_init_new(&skey);
    memset(&src, 0, sizeof(knodes));
    memset(&dst, 0, sizeof(knodes));

    // Check ping v4
    l = sizeof(b);
    rlpx_io_discovery_write_ping(&skey, 4, &src, &dst, 1234, b, &l);
    IF_ERR_EXIT(rlpx_io_parse_udp(b, l, &q, &type, &rlp));
    IF_ERR_EXIT(check_ping_v4(NULL, type, rlp));
    urlp_free(&rlp);

    // Check ping v5
    l = sizeof(b);
    rlpx_io_discovery_write_ping(&skey, 555, &src, &dst, 1234, b, &l);
    IF_ERR_EXIT(rlpx_io_parse_udp(b, l, &q, &type, &rlp));
    IF_ERR_EXIT(check_ping_v5(NULL, type, rlp));
    urlp_free(&rlp);

    // Check pong
    l = sizeof(b);
    urand(tmp.b, 32);
    rlpx_io_discovery_write_pong(&skey, &dst, &tmp, 1234, b, &l);
    IF_ERR_EXIT(rlpx_io_parse_udp(b, l, &q, &type, &rlp));
    IF_ERR_EXIT(check_pong(NULL, type, rlp));
    urlp_free(&rlp);

    // Check find node
    l = sizeof(b);
    rlpx_io_discovery_write_find(&skey, &skey.Q, 1234, b, &l);
    IF_ERR_EXIT(rlpx_io_parse_udp(b, l, &q, &type, &rlp));
    IF_ERR_EXIT(check_find_node(NULL, type, rlp));
    urlp_free(&rlp);

    // check neighbours
    l = sizeof(b);
    rlpx_io_discovery_write_neighbours(&skey, NULL, 1234, b, &l);
    IF_ERR_EXIT(rlpx_io_parse_udp(b, l, &q, &type, &rlp));
    IF_ERR_EXIT(check_neighbours(NULL, type, rlp));
    urlp_free(&rlp);

EXIT:
    uecc_key_deinit(&skey);
    if (rlp) urlp_free(&rlp);
    return err;
}

int
test_disc_read()
{
    urlp* rlp = NULL;
    uecc_public_key q;
    int type, err;

    // Check ping v4
    IF_ERR_EXIT(rlpx_io_parse_udp(g_ping_v4, g_ping_v4_sz, &q, &type, &rlp));
    IF_ERR_EXIT(check_ping_v4(NULL, type, rlp));
    urlp_free(&rlp);

    // Check ping v5
    IF_ERR_EXIT(rlpx_io_parse_udp(g_ping_v5, g_ping_v5_sz, &q, &type, &rlp));
    IF_ERR_EXIT(check_ping_v5(NULL, type, rlp));
    urlp_free(&rlp);

    // Check pong
    IF_ERR_EXIT(rlpx_io_parse_udp(g_pong, g_pong_sz, &q, &type, &rlp));
    IF_ERR_EXIT(check_pong(NULL, type, rlp));
    urlp_free(&rlp);

    // Check find node
    IF_ERR_EXIT(rlpx_io_parse_udp(g_find, g_find_node_sz, &q, &type, &rlp));
    IF_ERR_EXIT(check_find_node(NULL, type, rlp));
    urlp_free(&rlp);

    // check neighbours
    IF_ERR_EXIT(rlpx_io_parse_udp(g_peers, g_peers_sz, &q, &type, &rlp));
    IF_ERR_EXIT(check_neighbours(NULL, type, rlp));
    urlp_free(&rlp);

EXIT:
    return err;
}

int
test_disc_protocol()
{
    return 0;
}

int
check_ping_v4(ktable* t, int type, const urlp* rlp)
{
    ((void)t);
    int err = -1;
    int ver = urlp_as_u32(urlp_at(rlp, 0));
    uint32_t timestamp;
    uint8_t version[32];
    knodes src, dst;
    if (type != 1) return err;
    if (!(ver == 4)) return err;
    err = rlpx_io_discovery_recv_ping(&rlp, version, &src, &dst, &timestamp);
    return err;
}

int
check_ping_v5(ktable* t, int type, const urlp* rlp)
{
    ((void)t);
    int err = -1;
    int ver = urlp_as_u32(urlp_at(rlp, 0));
    uint32_t timestamp;
    uint8_t version[32];
    knodes src, dst;
    if (type != 1) return err;
    if (!(ver == 555)) return err;
    err = rlpx_io_discovery_recv_ping(&rlp, version, &src, &dst, &timestamp);
    return err;
}

int
check_pong(ktable* t, int type, const urlp* rlp)
{
    ((void)t);
    int err = -1;
    uint32_t timestamp;
    uint8_t echo[32];
    knodes dst;
    if (type != 2) return err;
    err = rlpx_io_discovery_recv_pong(&rlp, &dst, echo, &timestamp);
    return err;
}

int
check_find_node(ktable* t, int type, const urlp* rlp)
{
    ((void)t);
    int err = -1;
    if (type != 3) return err;
    uint32_t ts;
    uecc_public_key q;
    err = rlpx_io_discovery_recv_find(&rlp, &q, &ts);
    return err;
}

int
check_neighbours(ktable* t, int type, const urlp* rlp)
{
    int err = -1;
    if (type != 4) return err;
    err = rlpx_io_discovery_recv_neighbours(&rlp, check_neighbours_walk_fn, t);
    return err;
}

void
check_neighbours_walk_fn(const urlp* rlp, int idx, void* ctx)
{
    ((void)rlp);
    ((void)idx);
    ((void)ctx);
}
