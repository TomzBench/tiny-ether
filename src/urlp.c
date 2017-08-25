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

// private
urlp* urlp_alloc(uint32_t);  // init a rlp context on heap
uint32_t urlp_print_sz(uint8_t*, uint32_t*, uint32_t, const uint8_t);
uint32_t urlp_print_szsz(uint8_t*, uint32_t*, uint32_t, const uint8_t);
uint32_t urlp_szsz(uint32_t);  // size of size
uint32_t urlp_print_walk(urlp* rlp, uint8_t* b, uint32_t* spot);
// uint32_t urlp_print_internal(urlp* rlp, uint8_t* b, uint32_t* c, uint32_t
// sz);
// uint32_t urlp_scanlen_walk_fn(urlp* rlp, uint32_t* spot);
// uint32_t urlp_print_walk_fn(urlp* rlp, uint8_t* b, uint32_t* spot);

urlp* urlp_alloc(uint32_t sz) {
    urlp* rlp = NULL;
    rlp = urlp_malloc_fn(sizeof(urlp) + URLP_CONFIG_ANYSIZE_ARRAY + sz);
    if (rlp) {
	memset(rlp, 0, sizeof(urlp) + URLP_CONFIG_ANYSIZE_ARRAY + sz);
	rlp->sz = sz;
    }
    return rlp;
}

void urlp_free(urlp** rlp_p) {
    urlp* rlp = *rlp_p;
    *rlp_p = NULL;
    while (rlp) {
	urlp* delete = rlp;
	rlp = rlp->x;
	urlp_free_fn(delete);
    }
}

uint32_t urlp_print_sz(uint8_t* b, uint32_t* c, uint32_t s, const uint8_t p) {
    if (s <= 55) {
	if (b) b[--*(c)] = p + s;  // b[x] = 0x?? + sz
	return 1;
    } else {
	return urlp_print_szsz(b, c, s, p);
    }
}

uint32_t urlp_print_szsz(uint8_t* b, uint32_t* c, uint32_t s, const uint8_t p) {
    uint32_t szsz = urlp_szsz(s);
    uint8_t(*x)[4] = (uint8_t(*)[4])(&s);
    if (b) {  // caller may pass null to learn length
	for (int i = 0; i < szsz; i++) b[--*c] = *x[i];
	b[--*c] = p + szsz;
    }
    return szsz + 1;
}

uint32_t urlp_szsz(uint32_t size) { return 4 - (urlp_clz_fn(size) / 8); }

urlp* urlp_item(const uint8_t* b, uint32_t sz) {
    urlp* rlp = NULL;
    uint32_t size;
    if (sz == 0) {
	size = 1;
	rlp = urlp_alloc(size);
	if (rlp) rlp->b[--size] = 0x80;
    } else if (sz == 1 && b[0] < 0x80) {
	size = 1;
	rlp = urlp_alloc(size);
	if (rlp) rlp->b[--size] = b[0];
    } else if (sz <= 55) {
	size = sz + 1;
	rlp = urlp_alloc(size);
	if (rlp) {
	    for (int i = sz; i; i--) rlp->b[--size] = b[i - 1];
	    rlp->b[--size] = 0x80 + sz;
	}
    } else {
	size = urlp_szsz(sz) + 1 + sz;  // prefix + size of size + string
	rlp = urlp_alloc(size);
	if (rlp) {
	    for (int i = sz; i; i--) rlp->b[--size] = b[i - 1];
	    urlp_print_szsz(rlp->b, &size, sz, 0xb7);
	}
    }
    return rlp;
}

urlp* urlp_list(int n, ...) {
    va_list ap;
    va_start(ap, n);
    urlp* item = NULL;
    while (n--) {
	urlp* rlp = va_arg(ap, urlp*);
	urlp_push(item, rlp);
    }
    va_end(ap);
    return item;
}

urlp* urlp_push(urlp* dst, urlp* add) {
    if (!dst) dst = urlp_alloc(0);
    if (urlp_is_list(add)) {
	// list is always empty first node!
	// assert(!add->y)
	add->y = add->x;
    } else {
	// not lists always not linked!
	// assert(!add->x)
    }
    // first item in list always start with sz=0 node.
    if (!urlp_is_list(dst)) dst = urlp_push(urlp_alloc(0), dst);
    add->x = dst->x;
    dst->x = add;
    return dst;
}

uint32_t urlp_size(urlp* rlp) {
    return rlp->sz;  //
}

const uint8_t* urlp_data(urlp* rlp) {
    return rlp->b;  //
}

uint32_t urlp_print(urlp* rlp, uint8_t* b, uint32_t l) {
    uint32_t sz = urlp_print_walk(rlp, NULL, 0);  // get size
    if (!(sz <= l)) return sz;
    return urlp_print_walk(rlp, b, &sz);  // print if ok
}

uint32_t urlp_print_walk(urlp* rlp, uint8_t* b, uint32_t* spot) {
    uint32_t sz = 0, islist = urlp_is_list(rlp);
    while (rlp) {
	if (rlp->y) {
	    uint32_t inner = urlp_print_walk(rlp->y, b, spot);
	    sz += urlp_print_sz(b, spot, inner, 0xc0) + inner;
	}
	if (b) {
	    uint32_t rlpsz = rlp->sz;
	    while (rlpsz) b[--*(spot)] = rlp->b[--rlpsz];
	}
	sz += rlp->sz;
	rlp = rlp->x;
    }
    if (islist) sz += urlp_print_sz(b, spot, sz, 0xc0);
    return sz;
}
/*
uint32_t urlp_scanlen(urlp* rlp) {
    uint32_t spot = 0;
    urlp_scanlen_walk_fn(rlp, &spot);
    return spot;
}

uint32_t urlp_scanlen_walk_fn(urlp* rlp, uint32_t* spot) {
    uint32_t listsz = 0;
    urlp* start = rlp;
    while (rlp) {
	listsz += urlp_size(rlp);
	rlp = rlp->x;
    }
    if (urlp_is_list(start)) {  // cap item if this is a list
	listsz += urlp_print_sz(NULL, NULL, listsz, 0xc0);
    }
    *spot += listsz;
    return listsz;
}

uint32_t urlp_print(urlp* rlp, uint8_t* b, uint32_t l) {
    uint32_t size = urlp_scanlen(rlp);
    uint32_t spot = size;
    if (!(l <= size)) return size;
    urlp_print_walk_fn(rlp, b, &spot);
    return size;
}

uint32_t urlp_print_walk_fn(urlp* rlp, uint8_t* b, uint32_t* spot) {
    uint32_t listsz = 0;
    urlp* start = rlp = rlp->sz ? rlp : rlp->x;
    while (rlp) {
	uint32_t size = urlp_size(rlp);
	listsz += size;
	while (size) b[--*(spot)] = rlp->b[--size];
	rlp = rlp->x;
    }
    if (urlp_is_list(start)) {  // cap item if this is a list
	listsz += urlp_print_sz(b, spot, listsz, 0xc0);
    }
    return listsz;
}
*/

//
//
//
//
