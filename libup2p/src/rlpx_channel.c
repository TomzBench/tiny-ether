#include "rlpx_channel.h"
#include "rlpx_handshake.h"
#include "rlpx_helper_macros.h"
#include "unonce.h"

rlpx_channel*
rlpx_ch_alloc()
{
    return rlpx_ch_alloc_keypair(NULL, NULL);
}

rlpx_channel*
rlpx_ch_alloc_key(uecc_private_key* s)
{
    return rlpx_ch_alloc_keypair(s, NULL);
}

rlpx_channel*
rlpx_ch_alloc_keypair(uecc_private_key* s, uecc_private_key* e)
{
    rlpx_channel* session = rlpx_malloc_fn(sizeof(rlpx_channel));
    if (session) {
        // clean mem
        memset(session, 0, sizeof(rlpx_channel));

        // update info
        session->listen_port = 44;         // TODO
        memset(session->node_id, 'A', 65); // TODO

        // Create keys
        if (s) {
            uecc_key_init_binary(&session->skey, s);
        } else {
            uecc_key_init_new(&session->skey);
        }
        if (e) {
            uecc_key_init_binary(&session->ekey, e);
        } else {
            uecc_key_init_new(&session->ekey);
        }
    }
    return session;
}

void
rlpx_ch_free(rlpx_channel** session_p)
{
    rlpx_channel* s = *session_p;
    *session_p = NULL;
    uecc_key_deinit(&s->skey);
    uecc_key_deinit(&s->ekey);
    rlpx_free_fn(s);
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
                uint32_t sentlen,
                uint8_t* recv,
                uint32_t recvlen)
{
    int err;
    uint8_t buf[32 + ((sentlen > recvlen) ? sentlen : recvlen)],
        *out = &buf[32];
    if ((err = uecc_agree(&s->ekey, &s->remote_ekey))) return err;
    memcpy(buf, orig ? s->remote_nonce.b : s->nonce.b, 32);
    memcpy(out, orig ? s->nonce.b : s->remote_nonce.b, 32);

    // aes-secret / mac-secret
    ukeccak256(buf, 64, out, 32);        // h(nonces)
    memcpy(buf, &s->ekey.z.b[1], 32);    // (ephemeral || h(nonces))
    ukeccak256(buf, 64, out, 32);        // S(ephemeral || H(nonces))
    ukeccak256(buf, 64, out, 32);        // S(ephemeral || H(shared))
    uaes_init_bin(&s->aes_enc, out, 32); // aes-secret save
    uaes_init_bin(&s->aes_dec, out, 32); // aes-secret save
    ukeccak256(buf, 64, out, 32);        // S(ephemeral || H(aes-secret))
    uaes_init_bin(&s->aes_mac, out, 32); // mac-secret save

    // ingress / egress
    // Initiator egress-mac: sha3(mac-secret^recipient-nonce || auth-sent-init)
    //           ingress-mac: sha3(mac-secret^initiator-nonce || auth-recvd-ack)
    // Recipient egress-mac: sha3(mac-secret^initiator-nonce || auth-sent-ack)
    //           ingress-mac: sha3(mac-secret^recipient-nonce || auth-recv-init)
    // egress  = sha3(mac-secret^their nonce || cipher sent )
    // ingress = sha3(mac-secret^our nonce   || cipher received)
    ukeccak256_init(&s->emac);
    ukeccak256_init(&s->imac);
    XOR32_SET(buf, out, s->nonce.b); // (mac-secret^recepient-nonce);
    memcpy(&buf[32], recv, recvlen); // (m..^nonce)||auth-recv-init)
    ukeccak256_update(&s->imac, buf, 32 + recvlen); // S(m..^nonce)||auth-recv)
    XOR32(buf, s->nonce.b);                         // UNDO xor
    XOR32(buf, s->remote_nonce.b);                  // (mac-secret^nonce);
    memcpy(&buf[32], sent, sentlen); // (m..^nonce)||auth-sentd-init)
    ukeccak256_update(&s->emac, buf, 32 + sentlen); // S(m..^nonce)||auth-sent)

    return err;
}

//
//
//
