#libueth sources
set (sources 
	ueth.c)
set (headers
	include/ueth_config.h
	include/ueth.h)

#libueth
add_library(ueth ${sources} ${headers})
target_include_directories(ucrypto PUBLIC ./include)
target_include_directories(ucrypto PRIVATE ./)
target_link_libraries(ueth ucrypto up2p usys ucrypto urlp)
