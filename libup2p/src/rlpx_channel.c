#include "rlpx_channel.h"
#include "rlpx_handshake.h"
#include "rlpx_helper_macros.h"
#include "unonce.h"

#include "protocols/devp2p/rlpx_devp2p.h"

rlpx_channel*
rlpx_ch_alloc_keypair(uecc_private_key* skey, uecc_private_key* ekey)
{
    rlpx_channel* ch = rlpx_malloc(sizeof(rlpx_channel));
    if (ch) {
        rlpx_ch_init_keypair(ch, skey, ekey);
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
rlpx_ch_init_keypair(rlpx_channel* ch, uecc_private_key* s, uecc_private_key* e)
{
    // clean mem
    memset(ch, 0, sizeof(rlpx_channel));

    // update info
    ch->listen_port = 44;         // TODO
    memset(ch->node_id, 'A', 65); // TODO

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
}

uint64_t
rlpx_ch_version_remote(rlpx_channel* s)
{
    return s->remote_version;
}

const uecc_public_key*
rlpx_ch_pub_skey(rlpx_channel* s)
{
    return &s->skey.Q;
}

const uecc_public_key*
rlpx_ch_pub_ekey(rlpx_channel* s)
{
    return &s->ekey.Q;
}

const uecc_public_key*
rlpx_ch_remote_pub_ekey(rlpx_channel* s)
{
    return &s->remote_ekey;
}

const uecc_public_key*
rlpx_ch_remote_pub_skey(rlpx_channel* s)
{
    return &s->remote_skey;
}

uint32_t
rlpx_ch_listen_port(rlpx_channel* s)
{
    return s->listen_port;
}

const char*
rlpx_ch_node_id(rlpx_channel* s)
{
    return s->node_id;
}

int
rlpx_ch_auth_write(rlpx_channel* ch,
                   const uecc_public_key* to,
                   uint8_t* auth,
                   size_t* l)
{
    unonce(ch->nonce.b);
    return rlpx_auth_write(&ch->skey, &ch->ekey, &ch->nonce, to, auth, l);
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
rlpx_ch_ack_write(rlpx_channel* ch,
                  const uecc_public_key* to,
                  uint8_t* ack,
                  size_t* l)
{
    unonce(ch->nonce.b);
    return rlpx_ack_write(&ch->skey, &ch->ekey, &ch->nonce, to, ack, l);
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

    // ingress / egress
    // Initiator egress-mac: sha3(mac-secret^recipient-nonce || auth-sent-init)
    //           ingress-mac: sha3(mac-secret^initiator-nonce || auth-recvd-ack)
    // Recipient egress-mac: sha3(mac-secret^initiator-nonce || auth-sent-ack)
    //           ingress-mac: sha3(mac-secret^recipient-nonce || auth-recv-init)
    // egress  = sha3(mac-secret^their nonce || cipher sent )
    // ingress = sha3(mac-secret^our nonce   || cipher received)
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
rlpx_ch_hello_write(rlpx_channel* ch, uint8_t* out, size_t* l)
{
    size_t tmp = *l;
    int err = rlpx_devp2p_hello_write(ch->listen_port, ch->node_id, out, &tmp);
    if (!err) {
        err = rlpx_frame_write(&ch->x, 0, 0, out, tmp, out, l);
    }
    return err;
}

int
rlpx_ch_hello_read(rlpx_channel* ch, uint8_t* in, size_t l, urlp** p)
{
    return rlpx_frame_parse(&ch->x, in, l, p);
}

//
//
//
