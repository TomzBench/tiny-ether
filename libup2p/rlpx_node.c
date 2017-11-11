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

#include "rlpx_node.h"
#include "usys_io.h"

int rlpx_node_hex_to_bin(const char*, uint32_t, uint8_t*, uint32_t*);
int rlpx_node_bin_to_hex(const uint8_t*, uint32_t, char*, uint32_t*);

int
rlpx_node_init(
    rlpx_node* self,
    const uecc_public_key* id,
    const char* host,
    uint32_t tcp,
    uint32_t udp)
{
    memset(self, 0, sizeof(rlpx_node));
    if (strlen(host) <= 15) {
        uint32_t ip = usys_atoh(host);
        memcpy(self->ip_v4, &ip, 4);
        snprintf(self->ip_v4, sizeof(self->ip_v4), "%s", host);
        self->port_tcp = tcp;
        self->port_udp = udp;
        self->id = *id;
        return 0;
    } else {
        return -1;
    }
}

int
rlpx_node_init_enode(rlpx_node* self, const char* enode)
{
    uint32_t l;
    uint8_t raw[65] = { 0x04 };
    char host[16], *tcp = NULL, *udp = NULL;
    uecc_public_key key;

    if ((((l = strlen(enode)) < 136) || (l > 164)) ||            //
        (memcmp(enode, "enode://", 8)) ||                        //
        (!(enode[136] == '@')) ||                                //
        (rlpx_node_hex_to_bin(&enode[8], 128, &raw[1], NULL)) || //
        (!(tcp = memchr(&enode[136], ':', l - 136))) ||          //
        (uecc_btoq(raw, 65, &key))) {
        return -1;
    }
    udp = memchr(tcp, '.', &enode[l] - tcp); // optional
    memcpy(host, &enode[137], tcp - &enode[137]);
    host[tcp - &enode[137]] = 0;
    return rlpx_node_init(self, &key, host, atoi(++tcp), udp ? atoi(++udp) : 0);
}

void
rlpx_node_deinit(rlpx_node* self)
{
    memset(self, 0, sizeof(rlpx_node));
}

int
rlpx_node_hex_to_bin(
    const char* in,
    uint32_t inlen,
    uint8_t* out,
    uint32_t* outlen)
{

    uint32_t s = inlen ? inlen / 2 : strlen(in) / 2;
    if (!outlen) {
        outlen = &s;
    } else if (*outlen < s) {
        *outlen = s;
        return -1;
    }
    *outlen = s;
    for (size_t i = 0; i < *outlen; i++) {
        uint8_t c = 0;
        if (in[i * 2] >= '0' && in[i * 2] <= '9') c += (in[i * 2] - '0') << 4;
        if ((in[i * 2] & ~0x20) >= 'A' && (in[i * 2] & ~0x20) <= 'F')
            c += (10 + (in[i * 2] & ~0x20) - 'A') << 4;
        if (in[i * 2 + 1] >= '0' && in[i * 2 + 1] <= '9')
            c += (in[i * 2 + 1] - '0');
        if ((in[i * 2 + 1] & ~0x20) >= 'A' && (in[i * 2 + 1] & ~0x20) <= 'F')
            c += (10 + (in[i * 2 + 1] & ~0x20) - 'A');
        out[i] = c;
    }
    return 0;
}

int
rlpx_node_bin_to_hex(
    const uint8_t* in,
    uint32_t inlen,
    char* out,
    uint32_t* outlen)
{
    uint32_t s = (inlen * 2) + 1;
    const char* hex = "0123456789abcdef";
    if (!outlen) {
        outlen = &s;
    } else if (*outlen < s) {
        *outlen = s;
        return -1;
    }
    *outlen = s;

    for (uint32_t i = 0; i < inlen; i++) {
        out[i * 2] = hex[(in[i] >> 4) & 0xF];
        out[i * 2 + 1] = hex[in[i] & 0xF];
    }
    out[s - 1] = 0;
    return 0;
}
