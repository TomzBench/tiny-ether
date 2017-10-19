#libucrypto common
set(sources
	keccak-tiny/keccak-tiny.c
	uecies_decrypt.c
	uecies_encrypt.c
	unonce.c)
set(headers
	keccak-tiny/keccak-tiny.h
	keccak-tiny/ukeccak256.h
	uecies_encrypt.h
	uecies_decrypt.h
	unonce.h)
set(incdirs ${incdirs} keccak-tiny ./)

# libucrypto sha3 ecc
if(UETH_USE_SECP256K1)
	list(APPEND sources secp256k1/uecc.c secp256k1/uhash.c)
	list(APPEND headers secp256k1/uecc.h secp256k1/uhash.h)
	list(APPEND incdirs ./secp256k1)
	list(APPEND libs secp256k1)
	include(${CMAKE_SOURCE_DIR}/cmake/secp256k1.cmake)
endif()

# libucrypto rng aes
if(UETH_USE_MBEDTLS)
	include(${CMAKE_SOURCE_DIR}/cmake/mbedtls.cmake)
	list(APPEND sources mbedtls/uaes.c mbedtls/urand.c)
	list(APPEND headers mbedtls/uaes.h mbedtls/urand.h)
	list(APPEND incdirs ./mbedtls)
	list(APPEND libs mbedcrypto)
endif()

# see readme keccak-tiny
set(UETH_MEMSET_S_MACRO "memset_s(W,WL,V,OL)=memset(W,V,OL)")
add_definitions(-D"${UETH_MEMSET_S_MACRO}")

# add libraries
add_library(ucrypto ${sources} ${headers})
target_include_directories(ucrypto PUBLIC ${incdirs})
target_link_libraries(ucrypto ${libs})
#add_dependencies(ucrypto ${libs})

# build unit test
add_executable(ucrypto_unit_test test/test.c)
target_link_libraries(ucrypto_unit_test ucrypto)

# install unit test
install(TARGETS ucrypto_unit_test DESTINATION ${UETH_INSTALL_ROOT}/bin)
