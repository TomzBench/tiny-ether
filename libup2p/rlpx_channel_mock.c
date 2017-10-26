#include "rlpx_channel.h"

rlpx_channel*
rlpx_ch_mock_alloc(async_io_settings* s, uecc_ctx* skey)
{
    rlpx_channel* ch = rlpx_malloc(sizeof(rlpx_channel));
    if (ch) rlpx_ch_mock_init(ch, s, skey);
    return ch;
}

int
rlpx_ch_mock_init(rlpx_channel* ch, async_io_settings* settings, uecc_ctx* skey)
{
    rlpx_ch_init(ch, skey);
    // Override io
    if (settings->connect) ch->io.settings.connect = settings->connect;
    if (settings->ready) ch->io.settings.ready = settings->ready;
    if (settings->close) ch->io.settings.close = settings->close;
    if (settings->tx) ch->io.settings.tx = settings->tx;
    if (settings->rx) ch->io.settings.rx = settings->rx;
    return 0;
}
