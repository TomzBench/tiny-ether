add_executable(pingpong main.c)
target_link_libraries(pingpong ueth up2p usys ucrypto urlp)
install(TARGETS pingpong DESTINATION ${UETH_INSTALL_ROOT}/bin)
