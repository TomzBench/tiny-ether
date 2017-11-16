#klib project
# UPDATE_COMMAND "" when offline

ExternalProject_Add(klib-project
	PREFIX ${UETH_INSTALL_ROOT}
	GIT_REPOSITORY https://github.com/tomzok/klib
	GIT_TAG cmake
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
	UPDATE_COMMAND ""
)

# Trying not to recompile external projects so much, ideas welcome
set(KLIB_LIBRARY ${UETH_INSTALL_ROOT}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}klib${CMAKE_STATIC_LIBRARY_SUFFIX})
set(KLIB_INCLUDE_DIR ${UETH_INSTALL_ROOT}/include)
file(MAKE_DIRECTORY ${KLIB_INCLUDE_DIR})  # Must exist.

# create our target
add_library(klib INTERFACE)

# export headers
set_property(TARGET klib PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${KLIB_INCLUDE_DIR})
