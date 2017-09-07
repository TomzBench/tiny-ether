#ulib makefile

.PHONY: libs clean test print

libs:
	$(MAKE) -s -C ./liburlp
	$(MAKE) -s -C ./liburlp/test
	$(MAKE) -s -C ./libup2p
	$(MAKE) -s -C ./libup2p/test
	$(MAKE) -s -C ./libucrypto
	$(MAKE) -s -C ./libucrypto/test

clean:
	$(MAKE) -s -C ./liburlp clean
	$(MAKE) -s -C ./liburlp/test clean_app_test
	$(MAKE) -s -C ./libup2p clean
	$(MAKE) -s -C ./libup2p/test clean_app_test
	$(MAKE) -s -C ./libucrypto clean
	$(MAKE) -s -C ./libucrypto/test clean_app_test

test:
	$(MAKE) -s -C ./liburlp/test valgrind_app_test
	$(MAKE) -s -C ./libup2p/test valgrind_app_test
	$(MAKE) -s -C ./libucrypto/test valgrind_app_test
