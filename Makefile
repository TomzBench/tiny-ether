#ulib makefile

.PHONY: libs clean test print

libs:
	$(MAKE) -s -C ./liburlp
	$(MAKE) -s -C ./liburlp/test

clean:
	$(MAKE) -s -C ./liburlp clean
	$(MAKE) -s -C ./liburlp/test clean_app_test

test:
	$(MAKE) -s -C ./liburlp/test valgrind_app_test
