#secp256k1 project

if (MSVC)
    set(_only_release_configuration -DCMAKE_CONFIGURATION_TYPES=Release)
    set(_overwrite_install_command INSTALL_COMMAND cmake --build <BINARY_DIR> --config Release --target install)
endif()

ExternalProject_Add(secp256k1-project
	PREFIX ${UETH_INSTALL_ROOT}
	GIT_REPOSITORY https://github.com/thomaskey/secp256k1
	GIT_TAG develop
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
	           -DCMAKE_POSITION_INDEPENDENT_CODE=${BUILD_SHARED_LIBS}
	           -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
	           -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
	           ${_only_release_configuration}
	LOG_CONFIGURE 1
	LOG_INSTALL 1
	BUILD_COMMAND ""
	${_overwrite_install_command}
	EXCLUDE_FROM_ALL TRUE
)

# Trying not to recompile external projects so much, ideas welcome
set(SECP256K1_LIBRARY ${UETH_INSTALL_ROOT}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}secp256k1${CMAKE_STATIC_LIBRARY_SUFFIX})
set(SECP256K1_INCLUDE_DIR ${UETH_INSTALL_ROOT}/include)

add_library(secp256k1 STATIC IMPORTED)
add_custom_target(secp256k1-lib DEPENDS ${SECP256K1_LIBRARY})
add_dependencies(secp256k1 secp256k1-lib)
set_property(TARGET secp256k1 PROPERTY IMPORTED_CONFIGURATIONS Release)
set_property(TARGET secp256k1 PROPERTY IMPORTED_LOCATION_RELEASE ${SECP256K1_LIBRARY})
set_property(TARGET secp256k1 PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${SECP256K1_INCLUDE_DIR})

add_custom_command(
	OUTPUT "${SECP256K1_LIBRARY}"
	COMMAND make secp256k1-project)
