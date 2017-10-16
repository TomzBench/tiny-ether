#include "rlpx_channel.h"
#include "unonce.h"
#include "usys_log.h"

// Private io callbacks
int rlpx_ch_on_accept(void* ctx);
int rlpx_ch_on_connect(void* ctx);
int rlpx_ch_on_erro(void* ctx);
int rlpx_ch_on_send(void* ctx, int err, const uint8_t* b, uint32_t l);
int rlpx_ch_on_recv(void* ctx, int err, uint8_t* b, uint32_t l);

// Private protocol callbacks
int rlpx_ch_on_hello(void* ctx, const urlp* rlp);
int rlpx_ch_on_disconnect(void* ctx, const urlp* rlp);
int rlpx_ch_on_ping(void* ctx, const urlp* rlp);
int rlpx_ch_on_pong(void* ctx, const urlp* rlp);

// IO callback handlers
async_io_settings g_rlpx_ch_io_settings = { //
    .on_accept = rlpx_ch_on_accept,
    .on_connect = rlpx_ch_on_connect,
    .on_erro = rlpx_ch_on_erro,
    .on_send = rlpx_ch_on_send,
    .on_recv = rlpx_ch_on_recv
};

// Protocol callback handlers
rlpx_devp2p_protocol_settings g_devp2p_settings = { //
    .on_hello = rlpx_ch_on_hello,
    .on_disconnect = rlpx_ch_on_disconnect,
    .on_ping = rlpx_ch_on_ping,
    .on_pong = rlpx_ch_on_pong
};

rlpx_channel*
rlpx_ch_alloc(uecc_private_key* skey, uecc_private_key* ekey)
{
    rlpx_channel* ch = rlpx_malloc(sizeof(rlpx_channel));
    if (ch) {
        rlpx_ch_init(ch, skey, ekey);
    }
    return ch;
}

void
rlpx_ch_free(rlpx_channel** ch_p)
{
    rlpx_channel* ch = *ch_p;
    *ch_p = NULL;
    rlpx_ch_deinit(ch);
    rlpx_free(ch);
}

int
rlpx_ch_init(rlpx_channel* ch, uecc_private_key* s, uecc_private_key* e)
{
    // clean mem
    memset(ch, 0, sizeof(rlpx_channel));

    // Install network io handler
    async_io_init(&ch->io, ch, &g_rlpx_ch_io_settings);

    // update info
    ch->listen_port = 44;         // TODO
    memset(ch->node_id, 'A', 65); // TODO

    rlpx_devp2p_protocol_init(&ch->devp2p, &g_devp2p_settings);
    ch->protocols[0] = (rlpx_protocol*)&ch->devp2p;

    // Create keys
    if (s) {
        uecc_key_init_binary(&ch->skey, s);
    } else {
        uecc_key_init_new(&ch->skey);
    }
    if (e) {
        uecc_key_init_binary(&ch->ekey, e);
    } else {
        uecc_key_init_new(&ch->ekey);
    }
    return 0;
}

void
rlpx_ch_deinit(rlpx_channel* ch)
{
    uecc_key_deinit(&ch->skey);
    uecc_key_deinit(&ch->ekey);
    if (ch->hs) rlpx_handshake_free(&ch->hs);
}

void
rlpx_ch_nonce(rlpx_channel* ch)
{
    unonce(ch->nonce.b);
}

int
rlpx_ch_poll(rlpx_channel** ch, uint32_t count, uint32_t ms)
{
    return async_io_poll_n((async_io**)ch, count, ms);
}

int
rlpx_ch_connect(rlpx_channel* ch,
                const uecc_public_key* to,
                const char* host,
                uint32_t tcp)
{
    rlpx_node n;
    if (!rlpx_node_init(&n, to, host, tcp, 0)) {
        return rlpx_ch_connect_node(ch, &n);
    } else {
        return -1;
    }
}

int
rlpx_ch_connect_enode(rlpx_channel* ch, const char* enode)
{
    rlpx_node n;
    if (!rlpx_node_init_enode(&n, enode)) {
        return rlpx_ch_connect_node(ch, &n);
    } else {
        return -1;
    }
}

int
rlpx_ch_connect_node(rlpx_channel* ch, const rlpx_node* n)
{
    if (ch->hs) rlpx_handshake_free(&ch->hs);
    ch->hs = rlpx_handshake_alloc(1, &ch->skey, &ch->ekey, &ch->nonce, &n->id);
    if (ch->hs) {
        return async_io_connect(&ch->io, n->ip_v4, n->port_tcp) < 0 ? -1 : 0;
    } else {
        return -1;
    }
}

int
rlpx_ch_accept(rlpx_channel* ch, const uecc_public_key* from)
{
    if (ch->hs) rlpx_handshake_free(&ch->hs);
    ch->hs = rlpx_handshake_alloc(0, &ch->skey, &ch->ekey, &ch->nonce, from);
    if (ch->hs) {
        ch->io.sock = 3; // TODO ---
        async_io_memcpy(&ch->io, 0, ch->hs->cipher, ch->hs->cipher_len);
        return async_io_send(&ch->io);
    } else {
        return -1;
    }
}

int
rlpx_ch_recv(rlpx_channel* ch, const uint8_t* d, size_t l)
{
    int err, type;
    urlp* rlp = NULL;
    err = rlpx_frame_parse(&ch->x, d, l, &rlp);
    if (!err) {
        type = rlpx_frame_header_type(rlp);
        if (type >= 0 && type < 2) {
            err = ch->protocols[type]->recv(ch->protocols[type],
                                            rlpx_frame_body(rlp));
        }
        urlp_free(&rlp);
    }
    return err;
}

// TODO - free handshake context after receiving secrets.
int
rlpx_ch_recv_auth(rlpx_channel* ch, const uint8_t* b, size_t l)
{
    int err = 0;
    urlp* rlp = NULL;

    // Decrypt authentication packet (allocates rlp context)
    if ((err = rlpx_handshake_auth_recv(ch->hs, b, l, &rlp))) return err;

    // Process the Decrypted RLP data
    if (!(err = rlpx_handshake_auth_install(ch->hs, &rlp))) {
        err = rlpx_handshake_secrets(ch->hs, 0, &ch->x.emac, &ch->x.imac,
                                     &ch->x.aes_enc, &ch->x.aes_dec,
                                     &ch->x.aes_mac);
    }

    // Free rlp and return
    urlp_free(&rlp);
    return err;
}

int
rlpx_ch_recv_ack(rlpx_channel* ch, const uint8_t* ack, size_t l)
{
    int err = -1;
    urlp* rlp = NULL;

    // Decrypt authentication packet
    if ((err = rlpx_handshake_ack_recv(ch->hs, ack, l, &rlp))) return err;

    // Process the Decrypted RLP data
    if (!(err = rlpx_handshake_ack_install(ch->hs, &rlp))) {
        err = rlpx_handshake_secrets(ch->hs, 1, &ch->x.emac, &ch->x.imac,
                                     &ch->x.aes_enc, &ch->x.aes_dec,
                                     &ch->x.aes_mac);
    }

    // Free rlp and return
    urlp_free(&rlp);
    return err;
}

int
rlpx_ch_write_hello(rlpx_channel* ch, uint8_t* out, size_t* l)
{
    return rlpx_devp2p_protocol_write_hello(&ch->x, ch->listen_port,
                                            ch->node_id, out, l);
}

int
rlpx_ch_write_disconnect(rlpx_channel* ch,
                         RLPX_DEVP2P_DISCONNECT_REASON reason,
                         uint8_t* out,
                         size_t* l)
{
    return rlpx_devp2p_protocol_write_disconnect(&ch->x, reason, out, l);
}

int
rlpx_ch_write_ping(rlpx_channel* ch, uint8_t* out, size_t* l)
{
    return rlpx_devp2p_protocol_write_ping(&ch->x, out, l);
}

int
rlpx_ch_write_pong(rlpx_channel* ch, uint8_t* out, size_t* l)
{
    return rlpx_devp2p_protocol_write_pong(&ch->x, out, l);
}

int
rlpx_ch_on_accept(void* ctx)
{
    rlpx_channel* ch = (rlpx_channel*)ctx;
    usys_log_ok("p2p.accept %d", ch->io.sock);
    return 0;
}

int
rlpx_ch_on_connect(void* ctx)
{
    rlpx_channel* ch = (rlpx_channel*)ctx;
    usys_log_ok("p2p.connect %d", ch->io.sock);
    async_io_memcpy(&ch->io, 0, ch->hs->cipher, ch->hs->cipher_len);
    return async_io_send(&ch->io);
}

int
rlpx_ch_on_erro(void* ctx)
{
    rlpx_channel* ch = (rlpx_channel*)ctx;
    usys_log_ok("p2p.erro %d", ch->io.sock);
    return 0;
}

int
rlpx_ch_on_send(void* ctx, int err, const uint8_t* b, uint32_t l)
{
    rlpx_channel* ch = (rlpx_channel*)ctx;
    usys_log_ok("p2p.send %d", ch->io.sock);
    return 0;
}

int
rlpx_ch_on_recv(void* ctx, int err, uint8_t* b, uint32_t l)
{
    rlpx_channel* ch = (rlpx_channel*)ctx;
    usys_log_ok("p2p.recv %d", ch->io.sock);
    return 0;
}

int
rlpx_ch_on_hello(void* ctx, const urlp* rlp)
{
}

int
rlpx_ch_on_disconnect(void* ctx, const urlp* rlp)
{
}

int
rlpx_ch_on_ping(void* ctx, const urlp* rlp)
{
}

int
rlpx_ch_on_pong(void* ctx, const urlp* rlp)
{
}

//
//
//
