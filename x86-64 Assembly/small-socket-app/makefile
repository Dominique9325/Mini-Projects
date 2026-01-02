clean:
	rm -f ./*.o asmtest asmtest_debug

all: asmtest debug

debug: clean asmtest_debug.o
	ld -g asmtest_debug.o -o asmtest_debug

asmtest_debug.o: asmtest.asm
	nasm -f elf64 -g -F dwarf asmtest.asm -o asmtest_debug.o
	readelf --debug-dump=info asmtest_debug.o | head

asmtest: clean asmtest.o
	ld -m elf_x86_64 asmtest.o -o asmtest

asmtest.o: asmtest.asm
	nasm -f elf64 asmtest.asm -o asmtest.o

