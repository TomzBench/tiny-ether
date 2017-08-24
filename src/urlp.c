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
	if (delete->child) urlp_free(&delete);
	urlp_free_fn(delete);
    }
}

uint32_t urlp_szsz(uint32_t size) { return 4 - (urlp_clz_fn(size) / 8); }
uint32_t urlp_print_szsz(uint8_t* b, uint32_t* c, uint32_t size, uint8_t p) {
    uint32_t szsz = urlp_szsz(size);
    uint8_t(*x)[4] = (uint8_t(*)[4])(&size);
    for (int i = 0; i < szsz; i++) b[--*c] = *x[i];
    b[--*c] = p + szsz;
    return szsz + 1;
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
    va_list ap;
    va_start(ap, n);
    urlp* item = NULL;
    while (n--) {
	urlp* rlp = va_arg(ap, urlp*);
	urlp_push(&item, &rlp);
    }
    va_end(ap);
    return item;
}

// if add sz > 0xc0 push child, else push next
urlp* urlp_push(urlp** dst_p, urlp** add_p) {
    urlp *dst = *dst_p, *add = *add_p;
    *add_p = NULL;  // steal callers pointer. (caller no longer own.)
    if (!dst) *dst_p = dst = urlp_alloc(0);  // give caller root
    if (urlp_is_list(add)) {
	add->child = dst->child;
	dst->child = add;
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

uint32_t urlp_scanlen(urlp* rlp) {
    uint32_t spot = 0;
    urlp_scanlen_walk_fn(rlp->sz ? rlp : rlp->next, &spot);
    return spot;
}

uint32_t urlp_scanlen_walk_fn(urlp* rlp, uint32_t* spot) {
    uint32_t listsz = 0;
    urlp* start = rlp;
    while (rlp) {
	if (rlp->child) {
	    uint32_t innersz = urlp_scanlen_walk_fn(rlp, spot);
	    listsz += (innersz <= 55) ? 1 : 1 + urlp_szsz(innersz);
	}
	listsz += urlp_size(rlp);
	rlp = rlp->next;
    }
    if (start->next) {  // cap item if this is a list
	listsz += (listsz <= 55) ? 1 : 1 + urlp_szsz(listsz);
    }
    *spot += listsz;
    return listsz;
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
	if (rlp->child) {
	    uint32_t innersz = urlp_print_walk_fn(rlp, b, spot);
	    if (innersz <= 55) {
		b[--*(spot)] = 0xc0 + innersz;
		listsz++;
	    } else {
		listsz += urlp_print_szsz(b, spot, innersz, 0xc0);
	    }
	}
	uint32_t size = urlp_size(rlp);
	listsz += size;
	while (size) b[--*(spot)] = rlp->b[--size];
	rlp = rlp->next;
    }
    if (start->next) {  // cap item if this is a list
	if (listsz <= 55) {
	    b[--*(spot)] = 0xc0 + listsz++;
	} else {
	    listsz += urlp_print_szsz(b, spot, listsz, 0xc0);
	}
    }
    return listsz;
}

//
//
//
//
