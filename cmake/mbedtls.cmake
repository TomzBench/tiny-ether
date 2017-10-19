#mbedtls project

ExternalProject_Add(mbedtls
	PREFIX ${CMAKE_SOURCE_DIR}/target
	GIT_REPOSITORY https://github.com/ARMmbed/mbedtls
	GIT_TAG master
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
	           -DCMAKE_POSITION_INDEPENDENT_CODE=${BUILD_SHARED_LIBS}
	           -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
	           -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
	           ${_only_release_configuration}
	LOG_CONFIGURE 1
	BUILD_COMMAND ""
	${_overwrite_install_command}
	LOG_INSTALL 1
)
