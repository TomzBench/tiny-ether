#ulib makefile

.PHONY: libs clean test print

libs:
	#$(MAKE) -s -C ./board/linux-emu
	$(MAKE) -s -C ./micro-rlp
	$(MAKE) -s -C ./micro-rlp/test

clean:
	$(MAKE) -s -C ./micro-rlp clean
	$(MAKE) -s -C ./micro-rlp/test clean

test:
	$(MAKE) -s -C ./micro-rlp/test valgrind
