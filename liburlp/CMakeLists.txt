# add files
file(GLOB sources "*.c")
file(GLOB headers "*.h")

# liburlp library and public headers
add_library(urlp ${sources} ${headers})
target_include_directories(urlp PUBLIC ./)

# unit test for liburlp
add_executable(urlp_unit_test test/test.c)

# setup unit test dependencies
target_link_libraries(urlp_unit_test urlp)
add_dependencies(urlp_unit_test urlp)

# install unit test
install(TARGETS urlp_unit_test DESTINATION ${UETH_INSTALL_ROOT}/bin)
