all: copy example

elf_support.o: elf_support.c
	gcc -g -c elf_support.o ../elf_support.c

copy.o: copy.c
	gcc -g -c copy.o copy.c

copy: copy.o elf_support.o 
	gcc -g -o copy copy.o elf_support.o

example: ../example.c
	gcc -o example ../example.c

clean:
	rm -f *.o copy example ModifiedElfOutput/*