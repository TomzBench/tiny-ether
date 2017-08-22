#include "mtm/urlp.h"

int test_item(uint8_t *expect, uint32_t expect_len, char *src, uint32_t slen);
int test_list();

int main(int argc, char *argv[]) {
    int err = 0;

    err = test_item((uint8_t *)"\x83""cat", 4, "cat", 3);
    if (err) return err;

    err = test_item((uint8_t *)"\x83""dog", 4, "dog", 3);
    if (err) return err;

    err = test_item((uint8_t *)			     //
		    "\xb8\x38"			     //
		    "Lorem ipsum dolor sit amet, "   //
		    "consectetur adipisicing elit",  //
		    58,				     //
		    "Lorem ipsum dolor sit amet, "   //
		    "consectetur adipisicing elit",  //
		    56);
    if (err) return err;

    err = test_list();
    if (err) return err;

    return 0;
}

int test_item(uint8_t *expect, uint32_t expect_len, char *src, uint32_t slen) {
    uint8_t buffer[1024];
    int len;
    urlp_encoder enc;
    urlp_encoder_init(&enc, buffer, 1024);

    len = urlp_item_uint8(&enc, src, slen);  // -> 4
    if (!(len == expect_len)) return -1;
    if (!(urlp_size(&enc) == expect_len)) return -2;
    if (memcmp(expect, urlp_data(&enc), len)) return -3;
    return 0;
}

int test_list() {
    int len;
    uint8_t buffer[1024];
    urlp_encoder enc;
    urlp_encoder_init(&enc, buffer, 1024);
    len = urlp_list(&enc, 2,			      //
		    urlp_item_uint8(&enc, "cat", 3),  //
		    urlp_item_uint8(&enc, "dog", 3)   //
		    );
    if (!(len == 9)) return -1;
    if (memcmp(urlp_data(&enc),
	       "\xc8\x83"
	       "cat"
	       "\x83"
	       "dog",
	       9)) {
	return -1;
    }
    return 0;
}

//
//
//
