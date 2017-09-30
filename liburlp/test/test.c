#include "urlp.h"

uint8_t rlp_null[] = { '\x80' };
uint8_t rlp_null2[] = { '\xc2', '\x80', '\x80' };
uint8_t rlp_15[] = { '\x0f' };
uint8_t rlp_1024[] = { '\x82', '\x04', '\x00' };
uint8_t rlp_empty[] = { '\xc0' };
uint8_t rlp_empty_empty[] = { '\xc2', '\xc0', '\xc0' };
uint8_t rlp_empty_nest[] = { '\xc2', '\xc1', '\xc0' };
uint8_t rlp_cat[] = { '\x83', 'c', 'a', 't' };
uint8_t rlp_dog[] = { '\x83', 'd', 'o', 'g' };
uint8_t rlp_catdog[] = { '\xc8', '\x83', 'c', 'a', 't', '\x83', 'd', 'o', 'g' };
uint8_t rlp_max64[] = { '\x84', '\xff', '\xff', '\xff', '\xff' };
uint8_t rlp_half64[] = { '\x84', '\x01', '\x00', '\x00', '\x00' };
uint8_t rlp_max32[] = { '\x83', '\xff', '\xff', '\xff' };
uint8_t rlp_half32[] = { '\x83', '\x01', '\x00', '\x00' };
uint8_t rlp_max16[] = { '\x82', '\xff', '\xff' };
uint8_t rlp_half16[] = { '\x82', '\x01', '\x00' };
uint8_t rlp_catdogpig[] = {
    '\xcc',                //
    '\x83', 'c', 'a', 't', //
    '\x83', 'd', 'o', 'g', //
    '\x83', 'p', 'i', 'g'  //
};
uint8_t rlp_catdogpigcow[] = {
    '\xd2',                //
    '\xc8',                //
    '\x83', 'c', 'a', 't', //
    '\x83', 'd', 'o', 'g', //
    '\xc8',                //
    '\x83', 'p', 'i', 'g', //
    '\x83', 'c', 'o', 'w'  //
};
uint8_t rlp_lorem[] = {
    '\xb8', '\x38', 'L', 'o', 'r', 'e', 'm', ' ', 'i', 'p', 's', 'u',
    'm',    ' ',    'd', 'o', 'l', 'o', 'r', ' ', 's', 'i', 't', ' ',
    'a',    'm',    'e', 't', ',', ' ', 'c', 'o', 'n', 's', 'e', 'c',
    't',    'e',    't', 'u', 'r', ' ', 'a', 'd', 'i', 'p', 'i', 's',
    'i',    'c',    'i', 'n', 'g', ' ', 'e', 'l', 'i', 't' //
};

// TODO
uint8_t rlp_2lorem[] = {
    '\xb8', '\x38', 'L', 'o', 'r', 'e', 'm',    ' ',    'i', 'p', 's', 'u', 'm',
    ' ',    'd',    'o', 'l', 'o', 'r', ' ',    's',    'i', 't', ' ', 'a', 'm',
    'e',    't',    ',', ' ', 'c', 'o', 'n',    's',    'e', 'c', 't', 'e', 't',
    'u',    'r',    ' ', 'a', 'd', 'i', 'p',    'i',    's', 'i', 'c', 'i', 'n',
    'g',    ' ',    'e', 'l', 'i', 't', '\xb8', '\x38', 'L', 'o', 'r', 'e', 'm',
    ' ',    'i',    'p', 's', 'u', 'm', ' ',    'd',    'o', 'l', 'o', 'r', ' ',
    's',    'i',    't', ' ', 'a', 'm', 'e',    't',    ',', ' ', 'c', 'o', 'n',
    's',    'e',    'c', 't', 'e', 't', 'u',    'r',    ' ', 'a', 'd', 'i', 'p',
    'i',    's',    'i', 'c', 'i', 'n', 'g',    ' ',    'e', 'l', 'i', 't' //
};

uint8_t rlp_random[] = {
    '\xe1',                                               // [...
    '\x83', 'c',    'a', 't',                             // "cat"
    '\xc8', '\x83', 'c', 'a', 't', '\x83', 'd', 'o', 'g', // ["cat","dog"]
    '\x85', 'h',    'o', 'r', 's', 'e',                   // "horse"
    '\xc1', '\xc0',                                       // [[]]
    '\x83', 'p',    'i', 'g',                             // "pig"
    '\xc1', '\x80',                                       // [""]
    '\x85', 's',    'h', 'e', 'e', 'p'                    // "sheep"
};
uint8_t rlp_wat[] = { '\xc7', '\xc0', '\xc1', '\xc0',
                      '\xc3', '\xc0', '\xc1', '\xc0' };

int test_u8();
int test_u16();
int test_u32();
int test_u64();
int test_item(uint8_t*, uint32_t, urlp**);

int
main(int argc, char* argv[])
{
    int err = 0;
    err |= test_u8();
    err |= test_u16();
    err |= test_u32();
    err |= test_u64();
    return err;
}

int
test_u8()
{
    int err = 0;
    char* lorem = "Lorem ipsum dolor sit amet, consectetur adipisicing elit";
    urlp* rlp;

    // ""
    rlp = urlp_item("", 0);
    err |= test_item(rlp_null, sizeof(rlp_null), &rlp);

    // ["",""]
    rlp = urlp_list();
    urlp_push(rlp, urlp_item("", 0));
    urlp_push(rlp, urlp_item("", 0));
    err |= test_item(rlp_null2, sizeof(rlp_null2), &rlp);

    // 0x0f
    rlp = urlp_item("\x0f", 1);
    err |= urlp_as_u8(rlp) == 15 ? 0 : -1;
    err |= urlp_as_u16(rlp) == 15 ? 0 : -1;
    err |= urlp_as_u32(rlp) == 15 ? 0 : -1;
    err |= urlp_as_u64(rlp) == 15 ? 0 : -1;
    err |= test_item(rlp_15, sizeof(rlp_15), &rlp);

    // 0x400x00
    rlp = urlp_item("\x04\x00", 2);
    err |= urlp_as_u16(rlp) == 1024 ? 0 : -1;
    err |= urlp_as_u32(rlp) == 1024 ? 0 : -1;
    err |= urlp_as_u64(rlp) == 1024 ? 0 : -1;
    err |= test_item(rlp_1024, sizeof(rlp_1024), &rlp);

    // "cat"
    rlp = urlp_item("cat", 3);
    err |= memcmp(urlp_str(rlp), "cat", 3) ? -1 : 0;
    err |= test_item(rlp_cat, sizeof(rlp_cat), &rlp);

    // "dog"
    rlp = urlp_item("dog", 3);
    err |= test_item(rlp_dog, sizeof(rlp_dog), &rlp);

    // "lorem...
    rlp = urlp_item(lorem, 56);
    err |= test_item(rlp_lorem, sizeof(rlp_lorem), &rlp);

    // []
    rlp = urlp_list();
    err |= test_item(rlp_empty, sizeof(rlp_empty), &rlp);

    // [[],[]]
    rlp = urlp_list();
    urlp_push(rlp, urlp_list());
    urlp_push(rlp, urlp_list());
    err |= test_item(rlp_empty_empty, sizeof(rlp_empty_empty), &rlp);

    // [[[]]]
    rlp = urlp_push(urlp_list(), urlp_push(urlp_list(), urlp_list()));
    err |= test_item(rlp_empty_nest, sizeof(rlp_empty_nest), &rlp);

    //[[],[[]],[ [],[[]] ] ]
    rlp = urlp_list();
    urlp_push(rlp, urlp_list());
    urlp_push(rlp, urlp_push(urlp_list(), urlp_list()));
    urlp_push(rlp, urlp_push(urlp_push(urlp_list(), urlp_list()), //
                             urlp_push(urlp_list(), urlp_list())) //
              );
    err |= test_item(rlp_wat, sizeof(rlp_wat), &rlp);

    // ["cat","dog"]
    rlp = urlp_push(urlp_item("cat", 3), urlp_item("dog", 3));
    err |= test_item(rlp_catdog, sizeof(rlp_catdog), &rlp);

    // ["cat","dog","pig"]
    rlp = urlp_item("cat", 3);
    rlp = urlp_push(rlp, urlp_item("dog", 3));
    rlp = urlp_push(rlp, urlp_item("pig", 3));
    err |= memcmp(urlp_str(urlp_at(rlp, 0)), "cat", 3) ? -1 : 0;
    err |= memcmp(urlp_str(urlp_at(rlp, 1)), "dog", 3) ? -1 : 0;
    err |= memcmp(urlp_str(urlp_at(rlp, 2)), "pig", 3) ? -1 : 0;
    err |= test_item(rlp_catdogpig, sizeof(rlp_catdogpig), &rlp);

    // [["cat","dog"],["pig","cow"]]
    rlp = urlp_list();
    urlp_push(rlp, urlp_push(urlp_item("cat", 3), urlp_item("dog", 3)));
    urlp_push(rlp, urlp_push(urlp_item("pig", 3), urlp_item("cow", 3)));
    err |= test_item(rlp_catdogpigcow, sizeof(rlp_catdogpigcow), &rlp);

    // ["cat",["cat","dog"],"horse",[[]],"pig",[""],"sheep"]
    rlp = urlp_item("cat", 3);
    rlp = urlp_push(rlp, urlp_push(urlp_item("cat", 3), urlp_item("dog", 3)));
    urlp_push(rlp, urlp_item("horse", 5));
    urlp_push(rlp, urlp_push(NULL, urlp_list()));
    urlp_push(rlp, urlp_item("pig", 3));
    urlp_push(rlp, urlp_push(NULL, urlp_item("", 0)));
    urlp_push(rlp, urlp_item("sheep", 5));
    err |= test_item(rlp_random, sizeof(rlp_random), &rlp);

    return err;
}

int
test_u16()
{
    int err = 0;
    uint16_t cat[] = { 'c', 'a', 't' }; //
    uint16_t half[] = { 0x100 };
    uint16_t max[] = { 0xffff };
    uint16_t onefive[] = { 0x000f };
    urlp* rlp;

    rlp = urlp_item_u16(cat, 3);
    err |= test_item(rlp_cat, sizeof(rlp_cat), &rlp);

    rlp = urlp_item_u16(max, 1);
    err |= test_item(rlp_max16, sizeof(rlp_max16), &rlp);

    rlp = urlp_item_u16(half, 1);
    err |= test_item(rlp_half16, sizeof(rlp_half16), &rlp);

    rlp = urlp_item_u16(NULL, 0);
    err |= test_item(rlp_null, sizeof(rlp_null), &rlp);

    rlp = urlp_item_u16(onefive, 1);
    err |= test_item(rlp_15, sizeof(rlp_15), &rlp);

    return err;
}

int
test_u32()
{
    int err = 0;
    uint32_t cat[] = { 'c', 'a', 't' }; //
    uint32_t half[] = { 0x10000 };
    uint32_t max[] = { 0xffffff };
    uint32_t onefive[] = { 0x00000f };
    urlp* rlp;

    rlp = urlp_item_u32(cat, 3);
    err |= test_item(rlp_cat, sizeof(rlp_cat), &rlp);

    rlp = urlp_item_u32(max, 1);
    err |= test_item(rlp_max32, sizeof(rlp_max32), &rlp);

    rlp = urlp_item_u32(half, 1);
    err |= test_item(rlp_half32, sizeof(rlp_half32), &rlp);

    rlp = urlp_item_u32(NULL, 0);
    err |= test_item(rlp_null, sizeof(rlp_null), &rlp);

    rlp = urlp_item_u32(onefive, 1);
    err |= test_item(rlp_15, sizeof(rlp_15), &rlp);

    return err;
}

int
test_u64()
{
    int err = 0;
    uint64_t cat[] = { 'c', 'a', 't' }; //
    uint64_t half[] = { 0x1000000 };
    uint64_t max[] = { 0xffffffff };
    uint64_t onefive[] = { 0x0000000f };
    urlp* rlp;

    rlp = urlp_item_u64(cat, 3);
    err |= test_item(rlp_cat, sizeof(rlp_cat), &rlp);

    rlp = urlp_item_u64(max, 1);
    err |= test_item(rlp_max64, sizeof(rlp_max64), &rlp);

    rlp = urlp_item_u64(half, 1);
    err |= test_item(rlp_half64, sizeof(rlp_half64), &rlp);

    rlp = urlp_item_u64(NULL, 0);
    err |= test_item(rlp_null, sizeof(rlp_null), &rlp);

    rlp = urlp_item_u64(onefive, 1);
    err |= test_item(rlp_15, sizeof(rlp_15), &rlp);

    return err;
}

int
test_item(uint8_t* rlp, uint32_t rlplen, urlp** item_p)
{
    uint8_t result[rlplen];
    uint8_t resulta[rlplen];
    uint32_t len, ret = -1;
    urlp* item = *item_p;
    *item_p = NULL;

    // Check encoded
    len = urlp_print(item, result, rlplen);
    if (!(len == rlplen)) goto EXIT;
    if (memcmp(rlp, result, rlplen)) goto EXIT;
    urlp_free(&item);

    // Check our readback
    item = urlp_parse(result, rlplen);
    len = urlp_print(item, resulta, rlplen);
    if (!(len == rlplen)) goto EXIT;
    if (memcmp(rlp, result, rlplen)) goto EXIT;
    urlp_free(&item);

    memset(result, 0, rlplen);

    // Check decoded
    item = urlp_parse(rlp, rlplen);
    len = urlp_print(item, result, rlplen);
    if (!(len == rlplen)) goto EXIT;
    if (memcmp(rlp, result, rlplen)) goto EXIT;

    // Test pass
    ret = 0;
EXIT:
    if (item) urlp_free(&item);
    return ret;
}

//
//
//
