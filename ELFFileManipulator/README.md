#Repository for Editing ELF Files

### By Andrew Kosikowski and Daniel Jun Cho


Useful command for comparing hex in files:

cmp -l example ModifiedElfOutput/example | gawk '{printf "%08X %02X %02X\n", $1, strtonum(0$2), strtonum(0$3)}'


Valgrind because you are using C:

valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-out.txt ./add_elf_section example
