// Copyright 2017 Altronix Corp.
// This file is part of the tiny-ether library
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option) any later version.
//
// This program is distributed in the hope that it will be state,
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

#include "knodes.h"
#include "usys_io.h"

int
knodes_rlp_to_node(const urlp* rlp, knodes* ep)
{
    int err;
    uint32_t n = urlp_children(rlp);
    if (n < 3) return -1;
    // read in rlp to host format
    if ((!(err = urlp_idx_to_u32(rlp, 0, &ep->ip))) &&
        (!(err = urlp_idx_to_u32(rlp, 1, &ep->udp))) &&
        (!(err = urlp_idx_to_u32(rlp, 2, &ep->tcp)))) {
        return err;
    }
    return err;
}
urlp*
knodes_node_to_rlp(const knodes* ep)
{
    urlp* rlp = urlp_list();
    uint32_t ip, tcp, udp;
    // write rlp in network format
    ip = usys_htonl(ep->ip);
    tcp = usys_htons(ep->tcp);
    udp = usys_htons(ep->udp);
    if (rlp) {
        urlp_push_u8_arr(rlp, (uint8_t*)&ip, 4);
        urlp_push_u8_arr(rlp, (uint8_t*)&udp, 2);
        urlp_push_u8_arr(rlp, (uint8_t*)&tcp, 2);
        if (!(urlp_children(rlp) == 3)) urlp_free(&rlp);
    }
    return rlp;
}
