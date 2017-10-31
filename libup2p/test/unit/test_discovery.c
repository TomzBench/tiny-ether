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

uint32_t g_ping_v4_len;
uint32_t g_ping_v555_len;
uint32_t g_pong_len;
uint32_t g_find_node_len;
uint32_t g_neighbours_len;

const char* g_ping_v4 = DISCOVERY_PING_V4;
const char* g_ping_v555 = DISCOVERY_PING_V555;
const char* g_pong = DISCOVERY_PONG;
const char* g_find_node = DISCOVERY_FIND_NODE;
const char* g_neighbours = DISCOVERY_NEIGHBOURS;

uint8_t* g_ping_v4_bin = NULL;
uint8_t* g_ping_v555_bin = NULL;
uint8_t* g_pong_bin = NULL;
uint8_t* g_find_node_bin = NULL;
uint8_t* g_neighbours_bin = NULL;

// Encrypt a udp packet
int test_disc_write();

// Decrypt a udp packet
int test_disc_read();

// Test some protocol ops
int test_disc_protocol();

// check functions
typedef int (*check_fn)(rlpx_discovery_table*, int, const urlp*);
int check_ping_v4(rlpx_discovery_table* t, int type, const urlp* rlp);
int check_ping_v555(rlpx_discovery_table* t, int type, const urlp* rlp);
int check_pong(rlpx_discovery_table* t, int type, const urlp* rlp);
int check_find_node(rlpx_discovery_table* t, int type, const urlp* rlp);
int check_neighbours(rlpx_discovery_table* t, int type, const urlp* rlp);

int
test_discovery()
{
    int err = 0;

    // Init test vectors.
    g_ping_v4_len = strlen(g_ping_v4) / 2;
    g_ping_v555_len = strlen(g_ping_v555) / 2;
    g_pong_len = strlen(g_pong) / 2;
    g_find_node_len = strlen(g_find_node) / 2;
    g_neighbours_len = strlen(g_neighbours) / 2;
    g_ping_v4_bin = rlpx_malloc(g_ping_v4_len);
    g_ping_v555_bin = rlpx_malloc(g_ping_v555_len);
    g_pong_bin = rlpx_malloc(g_pong_len);
    g_find_node_bin = rlpx_malloc(g_find_node_len);
    g_neighbours_bin = rlpx_malloc(g_neighbours_len);
    memcpy(g_ping_v4_bin, makebin(g_ping_v4, NULL), g_ping_v4_len);
    memcpy(g_ping_v555_bin, makebin(g_ping_v555, NULL), g_ping_v555_len);
    memcpy(g_pong_bin, makebin(g_pong, NULL), g_pong_len);
    memcpy(g_find_node_bin, makebin(g_find_node, NULL), g_find_node_len);
    memcpy(g_neighbours_bin, makebin(g_neighbours, NULL), g_neighbours_len);

    err |= test_disc_read();
    err |= test_disc_write();
    err |= test_disc_protocol();

    // Free test vectors
    rlpx_free(g_ping_v4_bin);
    rlpx_free(g_ping_v555_bin);
    rlpx_free(g_pong_bin);
    rlpx_free(g_find_node_bin);
    rlpx_free(g_neighbours_bin);
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
    rlpx_discovery_table table;
    rlpx_discovery_endpoint from, to;

    // setup test
    rlpx_discovery_table_init(&table);
    rlpx_discovery_endpoint_v4_init(&from, 33342, 123, 456);
    rlpx_discovery_endpoint_v4_init(&to, 33342, 123, 456);
    uecc_key_init_new(&skey);

    // Check ping v4
    l = sizeof(b);
    rlpx_discovery_write_ping(&skey, 4, &from, &to, 1234, b, &l);
    IF_ERR_EXIT(rlpx_discovery_parse(b, l, &q, &type, &rlp));
    IF_ERR_EXIT(check_ping_v4(&table, type, rlp));
    urlp_free(&rlp);

    // Check ping v5
    l = sizeof(b);
    rlpx_discovery_write_ping(&skey, 555, &from, &to, 1234, b, &l);
    IF_ERR_EXIT(rlpx_discovery_parse(b, l, &q, &type, &rlp));
    IF_ERR_EXIT(check_ping_v555(&table, type, rlp));
    urlp_free(&rlp);

    // Check pong
    l = sizeof(b);
    urand(tmp.b, 32);
    rlpx_discovery_write_pong(&skey, &to, &tmp, 1234, b, &l);
    IF_ERR_EXIT(rlpx_discovery_parse(b, l, &q, &type, &rlp));
    IF_ERR_EXIT(check_pong(&table, type, rlp));
    urlp_free(&rlp);

    // Check find node
    l = sizeof(b);
    rlpx_discovery_write_find(&skey, &skey.Q, 1234, b, &l);
    IF_ERR_EXIT(rlpx_discovery_parse(b, l, &q, &type, &rlp));
    IF_ERR_EXIT(check_find_node(&table, type, rlp));
    urlp_free(&rlp);

    // check neighbours
    l = sizeof(b);
    rlpx_discovery_write_neighbours(&skey, &table, 1234, b, &l);
    IF_ERR_EXIT(rlpx_discovery_parse(b, l, &q, &type, &rlp));
    IF_ERR_EXIT(check_neighbours(&table, type, rlp));
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
    rlpx_discovery_table table;
    uecc_public_key nodeid;
    int type, err;
    rlpx_discovery_table_init(&table);

    // Construct test vector arrays for loop
    const uint8_t* reads[5] = { g_ping_v4_bin,
                                g_ping_v555_bin,
                                g_pong_bin,
                                g_find_node_bin,
                                g_neighbours_bin };
    uint32_t reads_sz[5] = { g_ping_v4_len,
                             g_ping_v555_len,
                             g_pong_len,
                             g_find_node_len,
                             g_neighbours_len };
    check_fn check[5] = { check_ping_v4,
                          check_ping_v555,
                          check_pong,
                          check_find_node,
                          check_neighbours };
    for (int i = 0; i < 5; i++) {
        err = rlpx_discovery_parse(reads[i], reads_sz[i], &nodeid, &type, &rlp);
        if (!err) {
            err = check[i](&table, type, rlp);
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
check_ping_v4(rlpx_discovery_table* t, int type, const urlp* rlp)
{
    ((void)t);
    int err = -1;
    int ver = urlp_as_u32(urlp_at(rlp, 0));
    uint32_t timestamp;
    uint8_t version[32];
    rlpx_discovery_endpoint from, to;
    if (type != 1) return err;
    if (!(ver == 4)) return err;
    err = rlpx_discovery_parse_ping(&rlp, version, &from, &to, &timestamp);
    return err;
}

int
check_ping_v555(rlpx_discovery_table* t, int type, const urlp* rlp)
{
    ((void)t);
    int err = -1;
    int ver = urlp_as_u32(urlp_at(rlp, 0));
    uint32_t timestamp;
    uint8_t version[32];
    rlpx_discovery_endpoint from, to;
    if (type != 1) return err;
    if (!(ver == 555)) return err;
    err = rlpx_discovery_parse_ping(&rlp, version, &from, &to, &timestamp);
    return err;
}

int
check_pong(rlpx_discovery_table* t, int type, const urlp* rlp)
{
    ((void)t);
    int err = -1;
    uint32_t timestamp;
    uint8_t echo[32];
    rlpx_discovery_endpoint to;
    if (type != 2) return err;
    err = rlpx_discovery_parse_pong(&rlp, &to, echo, &timestamp);
    return err;
}

int
check_find_node(rlpx_discovery_table* t, int type, const urlp* rlp)
{
    ((void)t);
    int err = -1;
    if (type != 3) return err;
    uint32_t ts;
    uecc_public_key q;
    err = rlpx_discovery_parse_find(&rlp, &q, &ts);
    return err;
}

int
check_neighbours(rlpx_discovery_table* t, int type, const urlp* rlp)
{
    int err = -1;
    if (type != 4) return err;
    err = rlpx_discovery_parse_neighbours(t, &rlp);
    return err;
}
