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

#include "rlpx_frame.h"
#include "rlpx_helper_macros.h"

// @brief Private methods

/**
 * @brief Authenticate and decrypt header frame
 *
 * @param x cipher secrets context data
 * @param header [in] input data to decrypt
 * @param header_urlp [out]
 * @param body_len [out] advertised frame length from header
 *
 * @return 0 OK -1 error
 */
int frame_parse_header(
    rlpx_coder* x,
    const uint8_t* header,
    urlp** header_urlp,
    uint32_t* body_len);

/**
 * @brief Authenticate and decrypt a body frame
 *
 * @param x cipher secrets context data
 * @param body [in] input data to decrypt
 * @param body_len [in] length of body data
 * @param rlp [out] allocated rlp list of body
 *
 * @return
 */
int frame_parse_body(
    rlpx_coder* x,
    const uint8_t* body,
    uint32_t body_len,
    urlp** rlp);

/**
 * @brief Update MAC state, convert plaintext to cipher text with MAC
 *
 * @param rlpx_coder [in] cipher secrets context data
 * @param x [in] egress data
 * @param xlen [in] egress data length
 * @param out [out] encrypted output
 * @param mac [out] mac
 *
 * @return 0 OK -1 error
 */
int frame_egress(
    rlpx_coder*,
    const uint8_t* x,
    size_t xlen,
    uint8_t* out,
    uint8_t* mac);

/**
 * @brief
 *
 * @param rlpx_coder [in] cipher secrets context data
 * @param x [in] ingress data
 * @param xlen [in] ingress data length
 * @param expect [in] MAC used to validate ingress data
 * @param out [out] decrypted data
 *
 * @return
 */
int frame_ingress(
    rlpx_coder*,
    const uint8_t* x,
    size_t xlen,
    const uint8_t* expect,
    uint8_t* out);

// public
int
rlpx_frame_write(
    rlpx_coder* x,
    uint32_t type,
    uint32_t id,
    uint8_t* data,
    size_t datalen,
    uint8_t* out,
    uint32_t* l)
{
    size_t len = AES_LEN(datalen);
    uint8_t head[32], body[len];
    if (*l < (32 + len + 16)) {
        *l = 32 + len + 16;
        return -1;
    }
    *l = 32 + len + 16;
    memcpy(body, data, datalen);
    memset(&body[datalen], 0, len - datalen);
    memset(head, 0, 32);
    WRITE_BE(3, head, (uint8_t*)&datalen);

    // TODO - fix rlpx.list(protocol-type[,context-id])
    head[3] = '\xc2', head[4] = '\x80' + type, head[5] = '\x80' + id;

    frame_egress(x, head, 0, out, &out[16]);
    frame_egress(x, body, len, &out[32], &out[32 + len]);
    return 0;
}

uint32_t
rlpx_frame_parse(rlpx_coder* x, const uint8_t* frame, size_t l, urlp** rlp_p)
{

    int err = 0;
    uint32_t sz;
    urlp *head = NULL, *body = NULL;

    if (l < 32) return 0;

    // Parse header
    err = frame_parse_header(x, frame, &head, &sz);
    if (err) return 0;

    // Check length (accounts for aes padding)
    if (l < (32 + (AES_LEN(sz)) + 16)) {
        urlp_free(&head);
        return 0;
    }

    // Parse body
    err = frame_parse_body(x, frame + 32, sz, &body);
    if (err) {
        urlp_free(&head);
        return 0;
    }

    // Return rlp frame to caller
    if (!*rlp_p) {
        *rlp_p = urlp_list();
        if (!*rlp_p) {
            urlp_free(&head);
            urlp_free(&body);
            return 0;
        }
    }
    urlp_push(*rlp_p, head);
    urlp_push(*rlp_p, body);
    return 32 + AES_LEN(sz) + 16;
}

int
frame_parse_header(
    rlpx_coder* x,
    const uint8_t* hdr,
    urlp** header_urlp,
    uint32_t* body_len)
{
    int err = -1;
    uint8_t tmp[32];

    err = frame_ingress(x, hdr, 0, &hdr[16], tmp);
    if (err) return err;

    // Read in big endian length prefix, give to caller
    *body_len = 0;
    READ_BE(3, body_len, tmp);

    // Parse header rlp, give to caller
    *header_urlp = urlp_parse(tmp + 3, 13);
    return *header_urlp ? 0 : -1;
}

int
frame_parse_body(rlpx_coder* x, const uint8_t* frame, uint32_t l, urlp** rlp)
{
    int err;
    uint8_t body[(l = l % 16 ? AES_LEN(l) : l)];
    err = frame_ingress(x, frame, l, frame + l, body);
    if (err) return err;

    if (body[0] < 0xc0) {
        // Some technical debt? Early packets did not nest their body frames
        // So we nest them here and pass up stack and we'll see how that goes
        *rlp = urlp_list();
        if (rlp) {
            urlp_push(*rlp, urlp_parse(body, 1));         // packet type
            urlp_push(*rlp, urlp_parse(body + 1, l - 1)); // packet data
        }
    } else {
        // This packet appears to be proper
        *rlp = urlp_parse(body, l);
    }
    return *rlp ? 0 : -1;
}

int
frame_egress(
    rlpx_coder* x,
    const uint8_t* plain,
    size_t xlen,
    uint8_t* out,
    uint8_t* mac)
{
    // encrypt
    // HEADER:
    // egress.update(aes(mac-secret,egress-mac) ^ header-ciphertext).digest
    //
    // FRAME:
    // egress-mac.update(aes(mac-secret,egress-mac) ^
    // left128(egress-mac.update(frame-ciphertext).digest))
    //
    // if xlen == 0 x is a header else x is a frame
    uint8_t xin[32], tmp[32];
    memset(xin, 0, 32);
    if (xlen) {
        if (uaes_crypt_ctr_update(&x->aes_enc, plain, xlen, out)) return -1;
        ukeccak256_update(&x->emac, (uint8_t*)out, xlen);
        ukeccak256_digest(&x->emac, xin);
    } else {
        if (uaes_crypt_ctr_update(&x->aes_enc, plain, 16, out)) return -1;
        memcpy(xin, out, 16);
    }
    ukeccak256_digest(&x->emac, tmp);          // egress-mac
    uaes_crypt_ecb_enc(&x->aes_mac, tmp, tmp); // aes(mac-secret,egress-mac)
    XORN(tmp, xin, 16);                        // aes(...)^cipher
    ukeccak256_update(&x->emac, tmp, 16);      // ingress(...)
    ukeccak256_digest(&x->emac, tmp);          // ingress(...).digest
    memcpy(mac, tmp, 16);
    return 0;
}

int
frame_ingress(
    rlpx_coder* x,
    const uint8_t* cipher,
    size_t xlen,
    const uint8_t* expect,
    uint8_t* out)
{
    // Check mac and decrypt
    // HEADER:
    // ingress.update(aes(mac-secret,ingress-mac) ^ header-ciphertext).digest
    //
    // FRAME: (ingress frame ciphertext before calling))
    // ingres-mac.update(aes(mac-secret,ingres-mac) ^
    // left128(ingres-mac.update(frame-ciphertext).digest))
    //
    // if xlen == 0 x is a header else x is a frame
    uint8_t tmp[32], xin[32];
    memset(xin, 0, 32);
    if (xlen) {
        ukeccak256_update(&x->imac, (uint8_t*)cipher, xlen);
        ukeccak256_digest(&x->imac, xin);
    } else {
        xlen = 16;
        memcpy(xin, cipher, 16);
    }
    ukeccak256_digest(&x->imac, tmp);          // ingress-mac
    uaes_crypt_ecb_enc(&x->aes_mac, tmp, tmp); // aes(mac-secret,ingress-mac)
    XORN(tmp, xin, 16);                        // aes(...)^cipher
    ukeccak256_update(&x->imac, tmp, 16);      // ingress(...)
    ukeccak256_digest(&x->imac, tmp);          // ingress(...).digest
    if (memcmp(tmp, expect, 16)) return -1;    // compare expect with actual
    return uaes_crypt_ctr_update(&x->aes_dec, cipher, xlen, out);
}

//
//
//
