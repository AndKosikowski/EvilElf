# Repository for Editing ELF Files

## By Andrew Kosikowski and Daniel Jun Cho

---

## Overview

This project is for editing ELF files without altering any functionality in order to allow them to evade detection from end-to-end deep learning malware detectors. The code is written in C using existing elf.h as a basis for our implementations of modifying and reading ELF files. This code was only tested on x64 least significant bit endianness ELF files but should work on all ELF files.

---

## Import 
The elf_support.c and elf_support.h are the key files to import to modify existing Linux ELF binaries. Add 
```include "elf_support.h" ``` and follow the compilations in the MAKEFILE to make sure the elf_support.c is compiled with it into an object file.

---


## Basic Usage
See the example .c files for usage of some of the functions in how to manipulate Linux ELF files. In general, you will need to run the following
```Elf_Manager* manager = load_elf_file(argv[1]);```

```Insert code here to modify manager (ELF File loaded in memory) ```

```write_elf_file(manager,argv[1]);```

```Insert code here for additional modifications that require directly writing to the file```

```free_manager(manager);```

The load and free are for setting up the ELF file in memory, while write_elf_file will write the current modified ELF file in memory to a file specified by the second argument.

---


## Helpful misc commands
Useful command for comparing hex in files:

cmp -l example ModifiedElfOutput/example | gawk '{printf "%08X %02X %02X\n", $1, strtonum(0$2), strtonum(0$3)}'


Valgrind because you are using C:

valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-out.txt ./add_elf_section example
