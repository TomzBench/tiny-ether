/**
 * @file urlp.c
 *
 * @brief Encode and decode bytes and lists of bytes into rlp format.
 *
 * Summary of encoding scheme
 *
 * [0x00,0x7f] single byte
 * [0x80,0xb7] 0-55 bytes
 * [0xb8,0xbf] 56-2^56 bytes
 * [0xc0,0xf7] 0-55 bytes of concatenated items.
 * [0xf8,0xff] 56-2^56 bytes of concatenated items.
 *
 * Where an "item" is described with first byte < 0xc0 and a "list" is described
 * with first byte >= 0xc0 and is concatenation of items.
 *
 * And the maximum size of an item or list is 2^56 - because the length of
 * length of bytes can be at most 7 bytes in length.
 *
 */

#include "urlp.h"

typedef struct {
    uint32_t sz, listsz, *spot;
    uint8_t* b;
} urlp_print_walk_fn_ctx;

// private
urlp* urlp_alloc(uint32_t);    // init a rlp context on heap
uint32_t urlp_szsz(uint32_t);  // size of size
uint32_t urlp_print_szsz(uint8_t*, uint32_t*, uint32_t, uint8_t);  // print szsz
uint32_t urlp_print_internal(urlp* rlp, uint8_t* b, uint32_t* c, uint32_t sz);
uint32_t urlp_scanlen_walk_fn(urlp* rlp, uint32_t* spot);
uint32_t urlp_print_walk_fn(urlp* rlp, uint8_t* b, uint32_t* spot);

urlp* urlp_alloc(uint32_t sz) {
    urlp* rlp = NULL;
    rlp = urlp_malloc_fn(sizeof(urlp) + URLP_CONFIG_ANYSIZE_ARRAY + sz);
    if (rlp) {
	memset(rlp, 0, sizeof(urlp) + URLP_CONFIG_ANYSIZE_ARRAY + sz);
	rlp->sz = rlp->spot = sz;
    }
    return rlp;
}

void urlp_free(urlp** rlp_p) {
    urlp* rlp = *rlp_p;
    *rlp_p = NULL;
    while (rlp) {
	urlp* delete = rlp;
	rlp = rlp->next;
	urlp_free_fn(delete);
    }
}

uint32_t urlp_szsz(uint32_t size) { return 4 - (urlp_clz_fn(size) / 8); }
uint32_t urlp_print_szsz(uint8_t* b, uint32_t* c, uint32_t size, uint8_t p) {
    uint32_t szsz = urlp_szsz(size);
    uint8_t(*x)[4] = (uint8_t(*)[4])(&size);
    for (int i = 0; i < szsz; i++) b[--*c] = *x[i];
    b[--*c] = p + szsz;
    return szsz;
}

urlp* urlp_item(uint8_t* b, uint32_t sz) {
    urlp* rlp = NULL;
    uint32_t size;
    if (sz == 0) {
	size = 1;
	rlp = urlp_alloc(size);
	if (rlp) rlp->b[--rlp->spot] = 0x80;
    } else if (sz == 1 && b[0] < 0x80) {
	size = 1;
	rlp = urlp_alloc(size);
	if (rlp) rlp->b[--rlp->spot] = b[0];
    } else if (sz <= 55) {
	size = sz + 1;
	rlp = urlp_alloc(size);
	if (rlp) {
	    for (int i = sz; i; i--) rlp->b[--rlp->spot] = b[i - 1];
	    rlp->b[--rlp->spot] = 0x80 + sz;
	}
    } else {
	size = urlp_szsz(sz) + 1 + sz;  // prefix + size of size + string
	rlp = urlp_alloc(size);
	if (rlp) {
	    for (int i = sz; i; i--) rlp->b[--rlp->spot] = b[i - 1];
	    urlp_print_szsz(rlp->b, &rlp->spot, sz, 0xb7);
	}
    }
    return rlp;
}

urlp* urlp_list(int n, ...) {
    //
}

// if add sz > 0xc0 push child, else push next
urlp* urlp_push(urlp** dst_p, urlp** add_p) {
    urlp *dst = *dst_p, *add = *add_p;
    *add_p = NULL;  // steal callers pointer. (caller no longer own.)
    if (!dst) *dst_p = dst = urlp_alloc(0);  // give caller root
    if (urlp_is_list(add)) {
    } else {
	add->next = dst->next;
	dst->next = add;
    }
    return dst;
}

uint32_t urlp_size(urlp* rlp) {
    return rlp->sz - rlp->spot;  //
}

uint8_t* urlp_data(urlp* rlp) {
    return &rlp->b[rlp->spot];  //
}

void urlp_walk(urlp* rlp, urlp_walk_fn fn, void* ctx) {
    while (rlp) {
	// if (rlp->child) urlp_walk(rlp->child, fn, ctx);
	fn(rlp, ctx);
	rlp = rlp->next;
    }
}

uint32_t urlp_scanlen(urlp* rlp) {
    uint32_t spot = 0;
    urlp_scanlen_walk_fn(rlp->sz ? rlp : rlp->next, &spot);
    return spot;
}

uint32_t urlp_scanlen_walk_fn(urlp* rlp, uint32_t* spot) {
    uint32_t listsz = 0;
    urlp* start = rlp;
    while (rlp) {
	listsz += urlp_size(rlp);
	rlp = rlp->next;
    }
    if (start->next) {  // cap item if this is a list
	*spot += listsz <= 55 ? listsz + 1 : listsz + 1 + urlp_szsz(listsz);
    } else {
	*spot += listsz;
    }
    return *spot;
}

uint32_t urlp_print(urlp* rlp, uint8_t* b, uint32_t l) {
    uint32_t size = urlp_scanlen(rlp);
    uint32_t spot = size;
    if (!(l <= size)) return size;
    urlp_print_walk_fn(rlp->sz ? rlp : rlp->next, b, &spot);
    return size;
}

uint32_t urlp_print_walk_fn(urlp* rlp, uint8_t* b, uint32_t* spot) {
    uint32_t listsz = 0;
    urlp* start = rlp;
    while (rlp) {
	uint32_t size = urlp_size(rlp);
	listsz += size;
	while (size) b[--*(spot)] = rlp->b[--size];
	rlp = rlp->next;
    }
    if (start->next) {  // cap item if this is a list
	if (listsz <= 55) {
	    b[--*(spot)] = 0xc0 + listsz;
	} else {
	    urlp_print_szsz(b, spot, listsz, 0xc0);
	}
    }
    return *spot;
}

/*
uint32_t urlp_print(urlp* rlp, uint8_t* b, uint32_t l) {
    uint32_t sz, size;
    urlp_print_walk_fn_ctx ctx = {.b = b, .listsz = 0};
    if (urlp_is_list(rlp)) {
	size = urlp_scanlen(rlp);
	ctx.spot = &size;
	sz = ctx.sz = size;
	urlp_walk(rlp, urlp_print_walk_fn, &ctx);
    } else {
	// Don't walk
	ctx.sz = size = sz = urlp_size(rlp);
	if (sz <= l) {
	    while (size) b[--ctx.sz] = rlp->b[--size];
	}
    }
    return sz;
}

void urlp_print_walk_fn(urlp* rlp, void* data) {
    urlp_print_walk_fn_ctx* ctx = data;			   // walk fn context
    uint32_t size = urlp_size(rlp);			   // size of rlp item
    ctx->listsz += size;				   // size of rlp list
    while (size) ctx->b[--*(ctx->spot)] = rlp->b[--size];  // write rlp
    if (!rlp->next) {					   // cap our list
	if (ctx->listsz <= 55) {
	    ctx->b[--*(ctx->spot)] = 0xc0 + ctx->listsz;
	} else {
	    urlp_print_szsz(ctx->b, ctx->spot, ctx->listsz, 0xc0);
	}
	ctx->listsz = 0;
    }
}
*/

// uint32_t urlp_print(urlp* rlp, uint8_t* buffer, uint32_t buffer_len) {
//    uint32_t spot = buffer_len;
//    urlp_print_internal(rlp, buffer, &spot, spot);
//}
//
// uint32_t urlp_print_internal(urlp* rlp, uint8_t* b, uint32_t* c, uint32_t sz)
// {
//    uint32_t size;
//    while (rlp) {
//	if (rlp->child) {
//	    // Print rlp and prefix list
//	    size = urlp_print_internal(rlp->child, b, c, sz);
//	    urlp_print_szsz(b, c, size, 0xc0);
//	}
//	size = urlp_size(rlp);
//	while (size) b[--*c] = rlp->b[--size];
//	rlp = rlp->next;
//    }
//    return sz - *c;
//}

// uint32_t urlp_print(urlp* rlp, uint8_t* b, uint32_t sz) {
//    uint32_t rlplen, ret = -1;
//    if ((rlplen = urlp_size(rlp)) <= sz) {
//	memcpy(b, urlp_data(rlp), rlplen);
//	ret = rlplen;
//    }
//    return ret;
//}

/*
uint32_t urlp_print_long_size(urlp_encoder *enc, uint32_t size, uint8_t pref);

int urlp_encoder_init(urlp_encoder *enc, uint8_t *bytes, uint32_t size) {
    memset(enc, 0, sizeof(urlp_encoder));
    enc->size = enc->spot = size;  // start out of range for writer.
    enc->bytes = bytes;
    return 0;
}

uint32_t urlp_item(urlp_encoder *enc, uint8_t *src, uint32_t slen) {
    uint32_t size;
    if (!enc->spot) return 0;
    if (slen == 0) {
	size = 1;
	enc->bytes[--enc->spot] = 0x80u;
    } else if (slen == 1 && src[0] < 0x80) {
	size = 1;
	enc->bytes[--enc->spot] = src[0];
    } else if (slen <= 55) {
	size = slen + 1;
	if (enc->spot - size < 0) return 0;  // negative result possible?
	for (int i = slen; i; i--) enc->bytes[--enc->spot] = src[i - 1];
	enc->bytes[--enc->spot] = 0x80 + slen;
    } else {
	for (int i = slen; i; i--) enc->bytes[--enc->spot] = src[i - 1];
	size = urlp_print_long_size(enc, slen, 0xb7) + slen + 1;
    }
    return size;
}

uint32_t urlp_list(urlp_encoder *enc, int n, ...) {
    uint32_t bytes = urlp_size(enc);
    if (bytes <= 55) {
	enc->bytes[--enc->spot] = 0xc0 + bytes++;
    } else {
	bytes += urlp_print_long_size(enc, bytes, 0xc0) + 1;
    }
    return bytes;
}

uint32_t urlp_print_long_size(urlp_encoder *enc, uint32_t size, uint8_t pref) {
    uint32_t szsz = 4 - (urlp_clz_fn(size) / 8);
    uint8_t(*b)[4] = (uint8_t(*)[4])(&size);
    for (int i = 0; i < szsz; i++) enc->bytes[--enc->spot] = *b[i];
    enc->bytes[--enc->spot] = pref + szsz;
    return szsz;
}

uint32_t urlp_size(urlp_encoder *enc) { return enc->size - enc->spot; }
uint8_t *urlp_data(urlp_encoder *enc) { return &enc->bytes[enc->spot]; }
*/

//
//
//
//
