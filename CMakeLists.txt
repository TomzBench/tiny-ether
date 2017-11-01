#------------------------------------------------------------------------------
# Top-level CMake file for micro-ether
#------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.5.1)

# modify default configs
if (NOT DEFINED UETH_CONFIG_FILE)
	set(UETH_CONFIG_FILE ${CMAKE_SOURCE_DIR}/cmake/default.cmake)
endif()

# other base includes includes
include(${UETH_CONFIG_FILE})
include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

# install targets go here
set(UETH_INSTALL_ROOT ${CMAKE_SOURCE_DIR}/target)
set(CMAKE_BUILD_TYPE Debug) #todo make a release option

# build libraries and their unit tests
add_subdirectory(libucrypto)
add_subdirectory(libueth)
add_subdirectory(libup2p)
add_subdirectory(liburlp)
add_subdirectory(libusys)

# some apps
##add_subdirectory(apps/pingpong)
