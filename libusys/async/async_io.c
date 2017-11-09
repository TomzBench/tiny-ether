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

#include "async_io.h"

void
async_io_init(async_io* io, void* ctx)
{
    io->sock = -1;
    io->ctx = ctx;
    io->close = usys_close;
    io->c = io->len = io->state = 0;
    memset(io->b, 0, sizeof(io->b));
}

void
async_io_deinit(async_io* io)
{
    if (async_io_has_sock(io)) async_io_close(io);
    memset(io->b, 0, sizeof(io->b));
}

int
async_io_poll_n(async_io** io, uint32_t n, uint32_t ms)
{
    uint32_t mask = 0;
    int reads[n], writes[n], err = 0;
    for (uint32_t c = 0; c < n; c++) {
        reads[c] = async_io_state_recv(io[c]) ? io[c]->sock : -1;
        writes[c] = async_io_state_send(io[c]) ? io[c]->sock : -1;
    }
    err = usys_select(&mask, &mask, ms, reads, n, writes, n);
    if (mask) {
        for (uint32_t i = 0; i < n; i++) {
            if (mask & (0x01 << i)) err |= async_io_poll(io[i]);
        }
    }
    return err;
}
