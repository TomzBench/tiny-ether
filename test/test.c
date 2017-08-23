#include "mtm/urlp.h"

uint8_t rlp_cat[] = {'\x83', 'c', 'a', 't'};
uint8_t rlp_dog[] = {'\x83', 'd', 'o', 'g'};
uint8_t rlp_catdog[] = {'\xc9', '\x83', 'c', 'a', 't', '\x83', 'd', 'o', 'g'};
uint8_t rlp_lorem[] = {'\xb8', '\x38', 'L', 'o', 'r', 'e', 'm', ' ', 'i', 'p',
		       's',    'u',    'm', ' ', 'd', 'o', 'l', 'o', 'r', ' ',
		       's',    'i',    't', ' ', 'a', 'm', 'e', 't', ',', ' ',
		       'c',    'o',    'n', 's', 'e', 'c', 't', 'e', 't', 'u',
		       'r',    ' ',    'a', 'd', 'i', 'p', 'i', 's', 'i', 'c',
		       'i',    'n',    'g', ' ', 'e', 'l', 'i', 't'};

int test_item(uint8_t *, uint32_t, uint8_t *, uint32_t);
int test_list(uint8_t *, uint32_t, int, ...);

int main(int argc, char *argv[]) {
    int err;

    err = test_item(rlp_cat, sizeof(rlp_cat), "cat", 3);
    err = test_item(rlp_dog, sizeof(rlp_dog), "dog", 3);
    err = test_item(rlp_lorem, sizeof(rlp_lorem),
		    "Lorem ipsum dolor sit amet, consectetur adipisicing elit",
		    56);
    err = test_list(rlp_catdog, sizeof(rlp_catdog), 2, "cat", 3, "dog", 3);

    return err;
}

int test_item(uint8_t *rlp, uint32_t rlplen, uint8_t *src, uint32_t srclen) {
    uint8_t result[rlplen];
    uint32_t len, ret = -1;
    urlp *item = urlp_item(src, srclen);
    len = urlp_print(item, result, rlplen);
    if (!(len == rlplen)) goto EXIT;
    if (!(urlp_size(item) == rlplen)) goto EXIT;
    if (memcmp(rlp, urlp_data(item), rlplen)) goto EXIT;
    if (memcmp(rlp, result, 4)) goto EXIT;
    ret = 0;
EXIT:
    urlp_free(&item);
    return ret;
}

int test_list(uint8_t *rlp, uint32_t rlplen, int n, ...) {
    uint8_t result[rlplen];
    uint32_t len, ret = -1;
    urlp *item = NULL;
    va_list ap;
    while (n--) {
	uint8_t *src = va_arg(ap, uint8_t *);
	uint32_t srclen = va_arg(ap, uint32_t);
	urlp_push(item, urlp_item(src, srclen));
    }
    va_end(ap);
    len = urlp_print(item, result, rlplen);
    if (!(len == rlplen)) goto EXIT;
    if (!(urlp_size(item) == len)) goto EXIT;
    if (memcmp(rlp, result, 4)) goto EXIT;
    ret = 0;
EXIT:
    urlp_free(&item);
    return ret;
}

/*
int test_str() {
    urlp rlp;

    //["cat",["puppy","cow"],"horse",[[]],"pig",[""],"sheep"]
    urlp_push(&rlp, urlp_item("cat", 3));		  // "cat",
    urlp_push(&rlp, urlp_list(2, "puppy", 5, "cow", 3));  // ["puppy","cow"]
    urlp_push(&rlp, urlp_item("horse", 5));		  // "horse
    urlp_push(&rlp, urlp_push(&rlp, urlp_list(0)));       // [[]]
    urlp_push(&rlp, urlp_item("pig", 3));		  // "pig"
    urlp_push(&rlp, urlp_list(1, "", 1));		  // [""]
    urlp_push(&rlp, urlp_item("sheep", 5));		  // "sheep"
    // urlp_print(&rlp);
    return 0;
}
*/

//
//
//
