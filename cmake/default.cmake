#default build unix host unix target

# libusys objs
set(UETH_TARGET_OS UNIX)

# libucrypto config
set(UETH_USE_LIBSECP256K1 TRUE)

# TODO depreciate these?
add_definitions(-DURLP_CONFIG_UNIX)
add_definitions(-DUSYS_CONFIG_UNIX)
