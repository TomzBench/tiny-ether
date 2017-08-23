#include "mtm/urlp.h"

int main(int argc, char *argv[]) {
    //
}

int test_str() {
    //["cat",["puppy","cow"],"horse",[[]],"pig",[""],"sheep"]
    urlp rlp;
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

//
//
//
