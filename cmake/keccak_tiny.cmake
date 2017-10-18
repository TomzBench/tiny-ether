# add files
set(sources ${UETH_EXTERNAL}/keccak-tiny/keccak-tiny.c)
set(headers ${UETH_EXTERNAL}/keccak-tiny/keccak-tiny.h)

# our main target
add_library(keccak-tiny ${sources} ${headers})


# see readme keccak-tiny
set(UETH_MEMSET_MACRO "memset_s(W,WL,V,OL)=memset(W,V,OL)")
add_definitions(-D"${UETH_MEMSET_MACRO}")

# includes
target_include_directories(keccak-tiny PUBLIC ./)
