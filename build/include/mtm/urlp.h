/**
 * @file urlp.h
 *
 * @brief api and types
 */
#ifndef URLP_H_
#define URLP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "urlp_config.h"

/**
 * @brief encoder context
 */
typedef struct {
    uint32_t size;  /* total size of user buffer */
    uint32_t spot;  /* internal use */
    uint8_t *bytes; /* encoded result*/
} urlp_encoder;

#define urlp_item_uint8(enc, b, l) urlp_item(enc, (uint8_t *)b, l)

/**
 * @brief Initialize encoder context
 *
 * @param encoder Context to initialize
 * @param bytes Pointer to your bytes array to store encoded data
 * @param size Size of your encoded data
 *
 * @return 0
 */
int urlp_encoder_init(urlp_encoder *encoder, uint8_t *bytes, uint32_t size);

/**
 * @brief Encode a single array of bytes with rlp prefix
 *
 * @param encoder Context
 * @param src Byte array to encode
 * @param slen Length of byte array you want to encode
 *
 * @return Size of encoded data or 0 if error
 */
uint32_t urlp_item(urlp_encoder *encoder, uint8_t *src, uint32_t slen);

/**
 * @brief Wrap array of items with rlp list prefix. Expects caller to know size
 * of the encoded data. Because urlp encode functions return sizes you can put
 * functions into the variadic arguments.
 *
 * TODO - compilers can evaluate function parameters in any order
 *
 * @code
 *  // Stack variables
 *  uint8_t buffer[1024];
 *  urlp_encoder ctx;
 *
 *  // Initialize encoder
 *  urlp_encoder_init(&ctx, buffer, 1024);
 *
 *  // Some animals ...["cat","dog"]
 *  urlp_list(&ctx, 2,
 *      urlp_item_uint8(&ctx, "cat", 3),
 *      urlp_item_uint8(&ctx, "dog", 3)
 *      );
 *
 *  // Some nested animals ["cat","dog",[["cat","dog"]]]
 *  urlp_list(&ctx, 3,
 *	      urlp_item_uint8(&ctx, "cat", 3),
 *	      urlp_item_uint8(&ctx, "dog", 3),
 *	      urlp_list(&ctx, 1,
 *			urlp_list(&ctx, 2,
 *			  	  urlp_item_uint8(&ctx, "cat", 3),
 *			  	  item_uint8(&ctx, "dog", 3)
 *			  	  )
 *		       )
 *	     );
 *
 * @endcode
 *
 * @param encode Context
 * @param n Number of items in list
 * @param ... N sizes of the items in the list
 *
 * @return Size of rlp encoded data of a list containing n items
 */
uint32_t urlp_list(urlp_encoder *encode, int n, ...);

/**
 * @brief Get the size of the rlp encoded data
 *
 * @param encoder Context
 *
 * @return Total size of rlp encoded data
 */
uint32_t urlp_size(urlp_encoder *encoder);

/**
 * @brief Get start of rlp encoded data
 *
 * @param encoder Context
 *
 * @return pointer to rlp encoded data or 0 if error
 */
uint8_t *urlp_data(urlp_encoder *encoder);

#ifdef __cplusplus
}
#endif
#endif
