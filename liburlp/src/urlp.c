/**
 * @file urlp.c
 *
 * @brief Encode and decode bytes and lists of bytes into rlp format.
 *
 * @code
 * 	uint8_t rlp_bytes[100]; ///Some stack for encoded rlp
 *
 * 	// ["cat","dog"] - rlp some pets
 * 	urlp *list = urlp_list();
 * 	urlp_push(list,"cat");
 * 	urlp_push(list,"dog");
 *
 *	// print out rlp  (0xc8, 0x83, 'c', 'a', 't', 0x83, ...)
 * 	urlp_print(list,rlp_bytes,100);
 * 	printf("%s\n",rlp_bytes); 
 *
 *	// free heap
 *	urlp_free(&list);
 */

#include "urlp.h"

/**
 * @brief urlp context
 * extra padding b[] for node rlp.
 */
typedef struct urlp {
    struct urlp *next, *child; /*!< list pointers FIFO */
    uint32_t sz;	       /*!< Number of bytes of rlp */
    uint8_t b[];	       /*!< Bytes of RLP stored here */
} urlp;

// private
uint32_t urlp_szsz(uint32_t);  // size of size
uint32_t urlp_write_sz(uint8_t*, uint32_t*, uint32_t, const uint8_t);
uint32_t urlp_write_szsz(uint8_t*, uint32_t*, uint32_t, const uint8_t);
uint32_t urlp_write_n_big_endian(uint8_t*, const void*, uint32_t, int);
uint32_t urlp_write_big_endian(uint8_t*, const void*, int);
uint32_t urlp_read_sz(uint8_t* b, uint32_t* result);
uint32_t urlp_print_walk(urlp* rlp, uint8_t* b, uint32_t* spot);
urlp* urlp_parse_walk(uint8_t* b, uint32_t l);

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
	rlp = rlp->next;
	if (delete->child) urlp_free(&delete->child);
	urlp_free_fn(delete);
    }
}

uint32_t urlp_szsz(uint32_t size) { return 4 - (urlp_clz_fn(size) / 8); }

uint32_t urlp_write_sz(uint8_t* b, uint32_t* c, uint32_t s, const uint8_t p) {
    if (s <= 55) {
	if (b) b[--*(c)] = p + s;  // ie: b[x] = 0xc0 + sz
	return 1;
    } else {
	return urlp_write_szsz(b, c, s, p);
    }
}

uint32_t urlp_write_szsz(uint8_t* b, uint32_t* c, uint32_t s, const uint8_t p) {
    uint32_t szsz = urlp_szsz(s);
    *c -= szsz;
    urlp_write_big_endian(&b[*c], &s, 4);
    if (b) b[--*c] = p + szsz;
    return szsz + 1;
}

uint32_t urlp_write_n_big_endian(uint8_t* b, const void* dat, uint32_t len,
				 int szof) {
    uint32_t spot = 0, n;
    while (len--) {
	n = urlp_write_big_endian(&b[spot], dat, szof);
	dat += szof;
	spot += n;
    }
    return spot;
}

uint32_t urlp_write_big_endian(uint8_t* b, const void* dat, int szof) {
    //[0x01,0x00,0x00,0x00] uint32_t int = 1; // little endian
    //[0x00,0x00,0x00,0x01] uint32_t int = 1; // big endian
    static int test = 1;		/*!< endianess test */
    uint8_t* x = (&((uint8_t*)dat)[0]); /*!< inner bytes ptr */
    int inc = 1;			/*!< ptr(++/--) */
    uint32_t c = 0;			/*!< Bytes written */
    int hit = 0;			/*!< start writing bytes */
    if (*(char*)&test) {		/*!< if little endian (start at end) */
	x = (&((uint8_t*)dat)[szof - 1]);
	inc = -1;
    }
    hit = 0;
    while (szof--) {
	if (*x) hit = 1;
	// If last byte, or already set msb or *x has weight.
	if (hit || *x || !szof) {
	    if (b) b[c] = *x;
	    c++;
	}
	x += inc;
    }
    return c;
}

uint32_t urlp_read_big_endian(void* dat, int szof, uint8_t* b) {
    static int test = 1;
    uint8_t* x = (&((uint8_t*)dat)[szof - 1]);
    int inc = -1;
    if (*(char*)&test == 0) { /*!< if we are big endian, read into mem.*/
	memcpy(dat, b, szof);
	return szof;
    }
    while (szof--) {
	*x += inc = *b++;
	x += inc;
    }
    return szof;
}

uint32_t urlp_read_sz(uint8_t* b, uint32_t* result) {
    uint32_t sz = 0, szsz = 0;
    *result = 0;
    if (*b <= 0x80) {
	*result = 1;
	sz = 0;
    } else if (*b <= 0xb7) {
	*result = *b - 0x80;
	sz = 1;
    } else if (*b <= 0xbf) {
	szsz = *b - 0xb7;
	urlp_read_big_endian(&sz, szsz, ++b);
	*result = sz;
	sz = 1 + szsz;
    } else if (*b == 0xc0) {
	*result = 1;
	sz = 0;
    } else if (*b <= 0xf7) {
	*result = *b - 0xc0;
	sz = 1;
    } else {
	szsz = *b - 0xc0;
	urlp_read_big_endian(&sz, szsz, ++b);
	*result = sz;
	sz = 1 + szsz;
    }
    return sz;
}

urlp* urlp_list() {
    return urlp_alloc(0);  //
}

urlp* urlp_item_u64(const uint64_t* b, uint32_t sz) {
    uint32_t blen = sz * sizeof(uint64_t);  // worstcase
    uint8_t bytes[blen];
    uint32_t len = urlp_write_n_big_endian(bytes, b, sz, sizeof(uint64_t));
    return urlp_item_u8(bytes, len);
}

urlp* urlp_item_u32(const uint32_t* b, uint32_t sz) {
    uint32_t blen = sz * sizeof(uint32_t);  // worstcase
    uint8_t bytes[blen];
    uint32_t len = urlp_write_n_big_endian(bytes, b, sz, sizeof(uint32_t));
    return urlp_item_u8(bytes, len);
}

urlp* urlp_item_u16(const uint16_t* b, uint32_t sz) {
    uint32_t blen = sz * sizeof(uint16_t);  // worstcase
    uint8_t bytes[blen];
    uint32_t len = urlp_write_n_big_endian(bytes, b, sz, sizeof(uint16_t));
    return urlp_item_u8(bytes, len);
}

urlp* urlp_item_u8(const uint8_t* b, uint32_t sz) {
    urlp* rlp = NULL;
    uint32_t size;
    if (sz == 0) {
	size = 1;
	rlp = urlp_alloc(size);
	if (rlp) rlp->b[--size] = 0x80;
    } else if (sz == 1 && b[0] <= 0x80) {
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
	    urlp_write_szsz(rlp->b, &size, sz, 0xb7);
	}
    }
    return rlp;
}

urlp* urlp_push(urlp* parent, urlp* child) {
    if (!parent) {
	parent = urlp_alloc(0);
    } else if (!urlp_is_list(parent)) {
	// first item in list always start with sz=0 node.
	// Note that we are changing the root node because caller is turning
	// a single item into a list... Might need better api to distingish this
	// to caller. We always return parent node to caller, but their original
	// callers parent node is now different. For safety we should not allow
	// this, and require caller to be more explicit when creating list or an
	// item... ie: if (!urlp_is_list(parent))return NULL; ...
	// Right now this code supports turning single items into list for them.
	parent = urlp_push(urlp_list(), parent);
    }
    if (parent->child) {
	child->next = parent->child;
	parent->child = child;
    } else {
	parent->child = child;
    }
    return parent;
}

uint32_t urlp_size(urlp* rlp) {
    return rlp->sz;  //
}

const uint8_t* urlp_data(urlp* rlp) {
    return rlp->b;  //
}

uint32_t urlp_print(urlp* rlp, uint8_t* b, uint32_t l) {
    uint32_t sz;
    if (!urlp_is_list(rlp)) {
	// handle case where this is single item and not a list
	if (!(rlp->sz <= l)) return rlp->sz;
	if (b) {
	    for (int i = rlp->sz - 1; i >= 0; i--) b[i] = rlp->b[i];
	}
	return rlp->sz;
    } else {
	if (rlp->child) {
	    // Regular list
	    sz = urlp_print_walk(rlp->child, NULL, 0);  // get size
	    if (!(sz <= l)) return sz;
	    return urlp_print_walk(rlp->child, b, &sz);  // print if ok
	} else {
	    // We have empty list... []
	    if (!(1 <= l)) return 1;  // size of 0xc0
	    if (b) *b = 0xc0;
	    return 1;
	}
    }
}

uint32_t urlp_print_walk(urlp* rlp, uint8_t* b, uint32_t* spot) {
    uint32_t sz = 0;
    while (rlp) {
	if (urlp_is_list(rlp)) {
	    if (rlp->child) {
		sz += urlp_print_walk(rlp->child, b, spot);
	    } else {  // empty list;
		sz += 1;
		if (b) b[--*(spot)] = 0xc0;
	    }
	}
	if (b) {
	    uint32_t rlpsz = rlp->sz;
	    while (rlpsz) b[--*(spot)] = rlp->b[--rlpsz];
	}
	sz += rlp->sz;
	rlp = rlp->next;
    }
    sz += urlp_write_sz(b, spot, sz, 0xc0);
    return sz;
}

urlp* urlp_parse(uint8_t* b, uint32_t l) {
    urlp* rlp = NULL;
    if (!b) return NULL;
    if (*b < 0xc0) {
	// Handle case where this is a single item and not a list
	uint32_t sz;
	b += urlp_read_sz(b, &sz);
	rlp = urlp_item(b, sz);
    } else {
	if (*b > 0xc0) {
	    // regular list
	    rlp = urlp_parse_walk(++b, l - 1);
	} else {
	    // empty list []
	    return urlp_list();
	}
    }
    return rlp;
}

urlp* urlp_parse_walk(uint8_t* b, uint32_t l) {
    urlp* rlp = NULL;
    uint8_t* end = &b[l];
    uint32_t sz;
    while (b < end) {
	if (*b >= 0xc0) {
	    // This is a list.
	    if (*b == 0xc0) {
		// Push empty list
		rlp = urlp_push(rlp, urlp_list());
		b++;
	    } else {
		// Push list of items into our list (recursive.)
		b += urlp_read_sz(b, &sz);
		rlp = urlp_push(rlp, urlp_parse_walk(b, sz));
		b += sz;
	    }
	} else {
	    // This is an item.
	    b += urlp_read_sz(b, &sz);
	    rlp = urlp_push(rlp, urlp_item(b, sz));
	    b += sz;
	}
    }
    return rlp;
}

//
//
//
//
