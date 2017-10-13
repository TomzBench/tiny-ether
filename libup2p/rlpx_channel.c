#include "rlpx_channel.h"
#include "rlpx_handshake.h"
#include "rlpx_helper_macros.h"
#include "unonce.h"

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

int
rlpx_ch_auth_load(rlpx_channel* ch, const uint8_t* auth, size_t l)
{
    int err = 0;
    urlp* rlp = NULL;

    // Decrypt authentication packet
    if ((err = rlpx_auth_read(&ch->skey, auth, l, &rlp))) return err;

    // Process the Decrypted RLP data
    err = rlpx_auth_load(&ch->skey, &ch->remote_version, &ch->remote_nonce,
                         &ch->remote_skey, &ch->remote_ekey, &rlp);

    // Free rlp and return
    urlp_free(&rlp);
    return err;
}

int
rlpx_ch_ack_load(rlpx_channel* ch, const uint8_t* ack, size_t l)
{
    int err = 0;
    urlp* rlp = NULL;

    // Decrypt acknowledge packet
    if ((err = rlpx_ack_read(&ch->skey, ack, l, &rlp))) return err;

    // Process the Decrypted RLP data
    err = rlpx_ack_load(&ch->remote_version, &ch->remote_nonce,
                        &ch->remote_ekey, &rlp);

    // Free rlp and return
    urlp_free(&rlp);
    return err;
}

int
rlpx_ch_secrets(rlpx_channel* s,
                int orig,
                uint8_t* sent,
                uint32_t slen,
                uint8_t* recv,
                uint32_t rlen)
{
    int err;
    uint8_t buf[32 + ((slen > rlen) ? slen : rlen)], *out = &buf[32];
    if ((err = uecc_agree(&s->ekey, &s->remote_ekey))) return err;
    memcpy(buf, orig ? s->remote_nonce.b : s->nonce.b, 32);
    memcpy(out, orig ? s->nonce.b : s->remote_nonce.b, 32);

    // aes-secret / mac-secret
    ukeccak256(buf, 64, out, 32);          // h(nonces)
    memcpy(buf, &s->ekey.z.b[1], 32);      // (ephemeral || h(nonces))
    ukeccak256(buf, 64, out, 32);          // S(ephemeral || H(nonces))
    ukeccak256(buf, 64, out, 32);          // S(ephemeral || H(shared))
    uaes_init_bin(&s->x.aes_enc, out, 32); // aes-secret save
    uaes_init_bin(&s->x.aes_dec, out, 32); // aes-secret save
    ukeccak256(buf, 64, out, 32);          // S(ephemeral || H(aes-secret))
    uaes_init_bin(&s->x.aes_mac, out, 32); // mac-secret save

    // Ingress / egress
    ukeccak256_init(&s->x.emac);
    ukeccak256_init(&s->x.imac);
    XOR32_SET(buf, out, s->nonce.b); // (mac-secret^recepient-nonce);
    memcpy(&buf[32], recv, rlen);    // (m..^nonce)||auth-recv-init)
    ukeccak256_update(&s->x.imac, buf, 32 + rlen); // S(m..^nonce)||auth-recv)
    XOR32(buf, s->nonce.b);                        // UNDO xor
    XOR32(buf, s->remote_nonce.b);                 // (mac-secret^nonce);
    memcpy(&buf[32], sent, slen); // (m..^nonce)||auth-sentd-init)
    ukeccak256_update(&s->x.emac, buf, 32 + slen); // S(m..^nonce)||auth-sent)

    return err;
}

int
rlpx_ch_send_auth(rlpx_channel* ch, const uecc_public_key* to)
{
    if (ch->hs) rlpx_handshake_free(&ch->hs);
    unonce(ch->nonce.b);
    ch->hs = rlpx_handshake_alloc_auth(
        &ch->skey, &ch->ekey, &ch->remote_version, &ch->nonce,
        &ch->remote_nonce, &ch->remote_skey, &ch->remote_ekey, to);
    if (ch->hs) {
        async_io_memcpy(&ch->io, 0, ch->hs->cipher, ch->hs->cipher_len);
        async_io_send(&ch->io);
        return 0;
    } else {
        return -1;
    }
}

int
rlpx_ch_write_auth(rlpx_channel* ch,
                   const uecc_public_key* to,
                   uint8_t* auth,
                   size_t* l)
{
    unonce(ch->nonce.b);
    return rlpx_auth_write(&ch->skey, &ch->ekey, &ch->nonce, to, auth, l);
}

int
rlpx_ch_send_ack(rlpx_channel* ch, const uecc_public_key* to)
{
    if (ch->hs) rlpx_handshake_free(&ch->hs);
    unonce(ch->nonce.b);
    ch->hs = rlpx_handshake_alloc_ack(&ch->skey, &ch->ekey, &ch->remote_version,
                                      &ch->nonce, &ch->remote_nonce,
                                      &ch->remote_skey, &ch->remote_ekey, to);
    if (ch->hs) {
        async_io_memcpy(&ch->io, 0, ch->hs->cipher, ch->hs->cipher_len);
        async_io_send(&ch->io);
        return 0;
    } else {
        return -1;
    }
}

int
rlpx_ch_write_ack(rlpx_channel* ch,
                  const uecc_public_key* to,
                  uint8_t* ack,
                  size_t* l)
{
    unonce(ch->nonce.b);
    return rlpx_ack_write(&ch->skey, &ch->ekey, &ch->nonce, to, ack, l);
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
rlpx_ch_read(rlpx_channel* ch, const uint8_t* d, size_t l)
{
    int err, type;
    urlp* rlp = NULL;
    err = rlpx_frame_parse(&ch->x, d, l, &rlp);
    if (!err) {
        type = rlpx_frame_header_type(rlp);
        if (type >= 0 && type < 2) {
            err = ch->protocols[type]->parse(ch->protocols[type],
                                             rlpx_frame_body(rlp));
        }
        urlp_free(&rlp);
    }
    return err;
}

int
rlpx_ch_on_accept(void* ctx)
{
}

int
rlpx_ch_on_connect(void* ctx)
{
}

int
rlpx_ch_on_erro(void* ctx)
{
}

int
rlpx_ch_on_send(void* ctx, int err, const uint8_t* b, uint32_t l)
{
}

int
rlpx_ch_on_recv(void* ctx, int err, uint8_t* b, uint32_t l)
{
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
