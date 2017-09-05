#ulib makefile

.PHONY: libs clean test print

libs:
	$(MAKE) -s -C ./liburlp
	$(MAKE) -s -C ./liburlp/test
	$(MAKE) -s -C ./libdevp2p
	$(MAKE) -s -C ./libdevp2p/test

clean:
	$(MAKE) -s -C ./liburlp clean
	$(MAKE) -s -C ./liburlp/test clean_app_test
	$(MAKE) -s -C ./libdevp2p clean
	$(MAKE) -s -C ./libdevp2p/test clean_app_test

test:
	$(MAKE) -s -C ./liburlp/test valgrind_app_test
	$(MAKE) -s -C ./libdevp2p/test valgrind_app_test
