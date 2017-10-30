#mbedtls project
# UPDATE_COMMAND "" when offline

if (MSVC)
    set(_only_release_configuration -DCMAKE_CONFIGURATION_TYPES=Release)
    set(_overwrite_install_command INSTALL_COMMAND cmake --build <BINARY_DIR> --config Release --target install)
endif()

ExternalProject_Add(mbedtls-project
	PREFIX ${UETH_INSTALL_ROOT}
	GIT_REPOSITORY https://github.com/ARMmbed/mbedtls
	GIT_TAG master
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
	           -DCMAKE_POSITION_INDEPENDENT_CODE=${BUILD_SHARED_LIBS}
	           -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
	           -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
		   -DENABLE_PROGRAMS=OFF
		   -DENABLE_TESTING=OFF
	           ${_only_release_configuration}
	UPDATE_COMMAND ""
	LOG_CONFIGURE 1
	LOG_INSTALL 1
	EXCLUDE_FROM_ALL TRUE
	${_overwrite_install_command}
)

# Trying not to recompile external projects so much, ideas welcome
set(MBEDCRYPTO_LIBRARY ${UETH_INSTALL_ROOT}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}mbedcrypto${CMAKE_STATIC_LIBRARY_SUFFIX})
set(MBEDCRYPTO_INCLUDE_DIR ${UETH_INSTALL_ROOT}/include)

add_library(mbedcrypto STATIC IMPORTED)
add_custom_target(mbedcrypto-lib DEPENDS ${MBEDCRYPTO_LIBRARY})
add_dependencies(mbedcrypto mbedcrypto-lib)
file(MAKE_DIRECTORY ${MBEDCRYPTO_INCLUDE_DIR})  # Must exist.
set_property(TARGET mbedcrypto PROPERTY IMPORTED_CONFIGURATIONS Release)
set_property(TARGET mbedcrypto PROPERTY IMPORTED_LOCATION_RELEASE ${MBEDCRYPTO_LIBRARY})
set_property(TARGET mbedcrypto PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${MBEDCRYPTO_INCLUDE_DIR})

add_custom_command(
	OUTPUT "${MBEDCRYPTO_LIBRARY}"
	COMMAND make mbedtls-project)
