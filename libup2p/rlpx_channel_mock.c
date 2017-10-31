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

#include "rlpx_channel.h"

rlpx_channel*
rlpx_ch_mock_alloc(async_io_settings* s, uecc_ctx* skey, const uint32_t* listen)
{
    rlpx_channel* ch = rlpx_malloc(sizeof(rlpx_channel));
    if (ch) rlpx_ch_mock_init(ch, s, skey, listen);
    return ch;
}

int
rlpx_ch_mock_init(
    rlpx_channel* ch,
    async_io_settings* settings,
    uecc_ctx* skey,
    const uint32_t* listen)
{
    rlpx_ch_init(ch, skey, listen);
    // Override io
    if (settings->connect) ch->io.settings.connect = settings->connect;
    if (settings->ready) ch->io.settings.ready = settings->ready;
    if (settings->close) ch->io.settings.close = settings->close;
    if (settings->tx) ch->io.settings.tx = settings->tx;
    if (settings->rx) ch->io.settings.rx = settings->rx;
    return 0;
}
