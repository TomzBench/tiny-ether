#include "mtm/urlp.h"

uint8_t rlp_null[] = {'\x80'};
uint8_t rlp_15[] = {'\x0f'};
uint8_t rlp_1024[] = {'\x82', '\x04', '\x00'};
uint8_t rlp_empty[] = {'\xc1', '\xc0'};
uint8_t rlp_cat[] = {'\x83', 'c', 'a', 't'};
uint8_t rlp_dog[] = {'\x83', 'd', 'o', 'g'};
uint8_t rlp_catdog[] = {'\xc8', '\x83', 'c', 'a', 't', '\x83', 'd', 'o', 'g'};
// ["cat","dog","pig"]
uint8_t rlp_catdogpig[] = {
    '\xcc',		    //
    '\x83', 'c', 'a', 't',  //
    '\x83', 'd', 'o', 'g',  //
    '\x83', 'p', 'i', 'g'   //
};
// [["cat","dog"],["pig","cow"]]
uint8_t rlp_catdogpigcow[] = {
    '\xd2',		    //
    '\xc8',		    //
    '\x83', 'c', 'a', 't',  //
    '\x83', 'd', 'o', 'g',  //
    '\xc8',		    //
    '\x83', 'p', 'i', 'g',  //
    '\x83', 'c', 'o', 'w'   //
};
uint8_t rlp_lorem[] = {'\xb8', '\x38', 'L', 'o', 'r', 'e', 'm', ' ', 'i', 'p',
		       's',    'u',    'm', ' ', 'd', 'o', 'l', 'o', 'r', ' ',
		       's',    'i',    't', ' ', 'a', 'm', 'e', 't', ',', ' ',
		       'c',    'o',    'n', 's', 'e', 'c', 't', 'e', 't', 'u',
		       'r',    ' ',    'a', 'd', 'i', 'p', 'i', 's', 'i', 'c',
		       'i',    'n',    'g', ' ', 'e', 'l', 'i', 't'};
uint8_t rlp_random[] = {
    '\xe1',						   // [...
    '\x83', 'c',    'a', 't',				   // "cat"
    '\xc8', '\x83', 'c', 'a', 't', '\x83', 'd', 'o', 'g',  // ["cat","dog"]
    '\x85', 'h',    'o', 'r', 's', 'e',			   // "horse"
    '\xc1', '\xc0',					   // [[]]
    '\x83', 'p',    'i', 'g',				   // "pig"
    '\xc1', '\x80',					   // [""]
    '\x85', 's',    'h', 'e', 'e', 'p'			   // "sheep"
};

int test_item(uint8_t *, uint32_t, urlp *);

int main(int argc, char *argv[]) {
    int err = 0;
    char *lorem = "Lorem ipsum dolor sit amet, consectetur adipisicing elit";
    urlp *rlp;

    rlp = urlp_item("", 0);
    err |= test_item(rlp_null, sizeof(rlp_null), rlp);
    urlp_free(&rlp);

    rlp = urlp_item("\x0f", 1);
    err |= test_item(rlp_15, sizeof(rlp_15), rlp);
    urlp_free(&rlp);

    rlp = urlp_item("\x04\x00", 2);
    err |= test_item(rlp_1024, sizeof(rlp_1024), rlp);
    urlp_free(&rlp);

    // rlp = urlp_alloc(0);
    // err |= test_item(rlp_empty, sizeof(rlp_empty), rlp);
    // urlp_free(&rlp);

    rlp = urlp_item("cat", 3);
    err |= test_item(rlp_cat, sizeof(rlp_cat), rlp);
    urlp_free(&rlp);

    rlp = urlp_item("dog", 3);
    err |= test_item(rlp_dog, sizeof(rlp_dog), rlp);
    urlp_free(&rlp);

    rlp = urlp_item(lorem, 56);
    err |= test_item(rlp_lorem, sizeof(rlp_lorem), rlp);
    urlp_free(&rlp);

    rlp = urlp_push(urlp_item("cat", 3), urlp_item("dog", 3));
    err |= test_item(rlp_catdog, sizeof(rlp_catdog), rlp);
    urlp_free(&rlp);

    rlp = urlp_item("cat", 3);
    rlp = urlp_push(rlp, urlp_item("dog", 3));
    rlp = urlp_push(rlp, urlp_item("pig", 3));
    err |= test_item(rlp_catdogpig, sizeof(rlp_catdogpig), rlp);
    urlp_free(&rlp);

    rlp = urlp_alloc(0);
    urlp_push(rlp, urlp_push(urlp_item("cat", 3), urlp_item("dog", 3)));
    urlp_push(rlp, urlp_push(urlp_item("pig", 3), urlp_item("cow", 3)));
    err |= test_item(rlp_catdogpigcow, sizeof(rlp_catdogpigcow), rlp);
    urlp_free(&rlp);

    /*
    err = test_item(rlp_random, sizeof(rlp_random), 7, //
		    urlp_item("cat", 3),				     //
		    urlp_list(2, urlp_item("cat", 3), urlp_item("dog", 3)),
		    urlp_item("horse", 5),				     //
		    urlp_list(1, urlp_list(0)), //
		    urlp_item("pig", 3),				     //
		    urlp_item("", 1), //
		    urlp_item("sheep", 5)				     //
		    );
		    */

    return err;
}

int test_item(uint8_t *rlp, uint32_t rlplen, urlp *item) {
    uint8_t result[rlplen];
    uint32_t len, ret = -1;
    len = urlp_print(item, result, rlplen);
    if (!(len == rlplen)) goto EXIT;
    if (memcmp(rlp, result, rlplen)) goto EXIT;
    ret = 0;
EXIT:
    return ret;
}

//
//
//
