# copied this from mbedtls cmake build dir.

set(src_crypto
	${UETH_EXTERNAL}/mbedtls/library/aes.c
    	${UETH_EXTERNAL}/mbedtls/library/aesni.c
    	${UETH_EXTERNAL}/mbedtls/library/arc4.c
    	${UETH_EXTERNAL}/mbedtls/library/asn1parse.c
    	${UETH_EXTERNAL}/mbedtls/library/asn1write.c
    	${UETH_EXTERNAL}/mbedtls/library/base64.c
    	${UETH_EXTERNAL}/mbedtls/library/bignum.c
    	${UETH_EXTERNAL}/mbedtls/library/blowfish.c
    	${UETH_EXTERNAL}/mbedtls/library/camellia.c
    	${UETH_EXTERNAL}/mbedtls/library/ccm.c
    	${UETH_EXTERNAL}/mbedtls/library/cipher.c
    	${UETH_EXTERNAL}/mbedtls/library/cipher_wrap.c
    	${UETH_EXTERNAL}/mbedtls/library/cmac.c
    	${UETH_EXTERNAL}/mbedtls/library/ctr_drbg.c
    	${UETH_EXTERNAL}/mbedtls/library/des.c
    	${UETH_EXTERNAL}/mbedtls/library/dhm.c
    	${UETH_EXTERNAL}/mbedtls/library/ecdh.c
    	${UETH_EXTERNAL}/mbedtls/library/ecdsa.c
    	${UETH_EXTERNAL}/mbedtls/library/ecjpake.c
    	${UETH_EXTERNAL}/mbedtls/library/ecp.c
    	${UETH_EXTERNAL}/mbedtls/library/ecp_curves.c
    	${UETH_EXTERNAL}/mbedtls/library/entropy.c
    	${UETH_EXTERNAL}/mbedtls/library/entropy_poll.c
    	${UETH_EXTERNAL}/mbedtls/library/error.c
    	${UETH_EXTERNAL}/mbedtls/library/gcm.c
    	${UETH_EXTERNAL}/mbedtls/library/havege.c
    	${UETH_EXTERNAL}/mbedtls/library/hmac_drbg.c
    	${UETH_EXTERNAL}/mbedtls/library/md.c
    	${UETH_EXTERNAL}/mbedtls/library/md2.c
    	${UETH_EXTERNAL}/mbedtls/library/md4.c
    	${UETH_EXTERNAL}/mbedtls/library/md5.c
    	${UETH_EXTERNAL}/mbedtls/library/md_wrap.c
    	${UETH_EXTERNAL}/mbedtls/library/memory_buffer_alloc.c
    	${UETH_EXTERNAL}/mbedtls/library/oid.c
    	${UETH_EXTERNAL}/mbedtls/library/padlock.c
    	${UETH_EXTERNAL}/mbedtls/library/pem.c
    	${UETH_EXTERNAL}/mbedtls/library/pk.c
    	${UETH_EXTERNAL}/mbedtls/library/pk_wrap.c
    	${UETH_EXTERNAL}/mbedtls/library/pkcs12.c
    	${UETH_EXTERNAL}/mbedtls/library/pkcs5.c
    	${UETH_EXTERNAL}/mbedtls/library/pkparse.c
    	${UETH_EXTERNAL}/mbedtls/library/pkwrite.c
    	${UETH_EXTERNAL}/mbedtls/library/platform.c
    	${UETH_EXTERNAL}/mbedtls/library/ripemd160.c
    	${UETH_EXTERNAL}/mbedtls/library/rsa.c
    	${UETH_EXTERNAL}/mbedtls/library/sha1.c
    	${UETH_EXTERNAL}/mbedtls/library/sha256.c
    	${UETH_EXTERNAL}/mbedtls/library/sha512.c
    	${UETH_EXTERNAL}/mbedtls/library/threading.c
    	${UETH_EXTERNAL}/mbedtls/library/timing.c
    	${UETH_EXTERNAL}/mbedtls/library/version.c
    	${UETH_EXTERNAL}/mbedtls/library/version_features.c
    	${UETH_EXTERNAL}/mbedtls/library/xtea.c
)

set(src_x509
	${UETH_EXTERNAL}/mbedtls/library/certs.c
    	${UETH_EXTERNAL}/mbedtls/library/pkcs11.c
    	${UETH_EXTERNAL}/mbedtls/library/x509.c
    	${UETH_EXTERNAL}/mbedtls/library/x509_create.c
    	${UETH_EXTERNAL}/mbedtls/library/x509_crl.c
    	${UETH_EXTERNAL}/mbedtls/library/x509_crt.c
    	${UETH_EXTERNAL}/mbedtls/library/x509_csr.c
    	${UETH_EXTERNAL}/mbedtls/library/x509write_crt.c
    	${UETH_EXTERNAL}/mbedtls/library/x509write_csr.c
)

set(src_tls
	${UETH_EXTERNAL}/mbedtls/library/debug.c
    	${UETH_EXTERNAL}/mbedtls/library/net_sockets.c
    	${UETH_EXTERNAL}/mbedtls/library/ssl_cache.c
    	${UETH_EXTERNAL}/mbedtls/library/ssl_ciphersuites.c
    	${UETH_EXTERNAL}/mbedtls/library/ssl_cli.c
    	${UETH_EXTERNAL}/mbedtls/library/ssl_cookie.c
    	${UETH_EXTERNAL}/mbedtls/library/ssl_srv.c
    	${UETH_EXTERNAL}/mbedtls/library/ssl_ticket.c
    	${UETH_EXTERNAL}/mbedtls/library/ssl_tls.c
)

if(CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wmissing-declarations -Wmissing-prototypes")
endif(CMAKE_COMPILER_IS_GNUCC)

if(CMAKE_COMPILER_IS_CLANG)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wmissing-declarations -Wmissing-prototypes -Wdocumentation -Wno-documentation-deprecated-sync -Wunreachable-code")
endif(CMAKE_COMPILER_IS_CLANG)

if(WIN32)
    set(libs ${libs} ws2_32)
endif(WIN32)

if(USE_PKCS11_HELPER_LIBRARY)
    set(libs ${libs} pkcs11-helper)
endif(USE_PKCS11_HELPER_LIBRARY)

if(ENABLE_ZLIB_SUPPORT)
    set(libs ${libs} ${ZLIB_LIBRARIES})
endif(ENABLE_ZLIB_SUPPORT)

if(LINK_WITH_PTHREAD)
    set(libs ${libs} pthread)
endif()

set(mbedcrypto_static_target "mbedcrypto_static")
add_library(${mbedcrypto_static_target} STATIC ${src_crypto})
set_target_properties(${mbedcrypto_static_target} PROPERTIES OUTPUT_NAME mbedcrypto)
target_link_libraries(${mbedcrypto_static_target} ${libs})
