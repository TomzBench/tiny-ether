#libup2p sources
set(sources
	rlpx_io.c
	rlpx_io_devp2p.c
	rlpx_discovery.c
	rlpx_frame.c
	rlpx_handshake.c
	kademlia/ktable.c
	rlpx_node.c
	rlpx_test_helpers.c)
set(headers
	kademlia/ktable.h
	rlpx_io.h
	rlpx_config.h
	rlpx_config_unix.h
	rlpx_io_devp2p.h
	rlpx_discovery.h
	rlpx_frame.h
	rlpx_handshake.h
	rlpx_helper_macros.h
	rlpx_node.h
	rlpx_test_helpers.h
	rlpx_types.h)

#libup2p unit test sources
set(sources-unit-test
	test/unit/test.c
	test/unit/test_discovery.c
	test/unit/test_enode.c
	test/unit/test_frame.c
	test/unit/test_handshake.c
	test/unit/test_kademlia.c
	test/unit/test_mock.c
	test/unit/test_protocol.c)
set(headers-unit-test
	test/unit/test.h
	test/unit/test_vectors.h)

#libp2p integration test sources
set(sources-integration-test 
	test/integration/test.c
	test/integration/test_enodes.h)

# lib
add_library(up2p ${sources} ${headers})
target_link_libraries(up2p usys ucrypto urlp)
target_include_directories(up2p PUBLIC ./)

#unit test for libup2p
add_executable(up2p_unit_test 
	${sources-unit-test}
	${headers-unit-test})
target_link_libraries(up2p_unit_test up2p)

#integration test for libup2p
add_executable(up2p_integration_test 
	${sources-integration-test}
	${headers-integration-test})
target_link_libraries(up2p_integration_test up2p)

# install unit test
install(TARGETS up2p_integration_test up2p_unit_test
	DESTINATION ${UETH_INSTALL_ROOT}/bin)

#
#
#
