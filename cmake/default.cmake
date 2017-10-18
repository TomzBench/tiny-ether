#default build unix host unix target

# libusys objs
set(UETH_TARGET_OS UNIX)

# libucrypto config
# TODO - add external modules
option(UETH_USE_MBEDTLS "Link with libmbedcrypto.a" ON)
option(UETH_USE_SECP256K1 "Link with libsecp256k1.a" ON)

# TODO depreciate these?
add_definitions(-DURLP_CONFIG_UNIX)
add_definitions(-DUSYS_CONFIG_UNIX)
