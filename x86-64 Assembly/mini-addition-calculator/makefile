.PHONY: clean

clean:
	rm *.o addition

all: addition

addition: addition.o
	ld -g addition.o -o addition

addition.o: addition.asm
	nasm -f elf64 addition.asm -o addition.o
