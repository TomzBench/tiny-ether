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

# TODO - use the libsecp256k1 repo add a cmake.txt
if(UETH_USE_SECP256K1)
	ExternalProject_Add(secp256k1
		INSTALL_DIR ${CMAKE_SOURCE_DIR}/target
		GIT_REPOSITORY https://github.com/chfast/secp256k1
	)
endif()
