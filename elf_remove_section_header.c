#include "elf_support.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//testing removing the section header table
int main(int argc, char** argv){
    if(argc < 2){
        printf("Need to specify a path to a file as an argument\n");
        return 1;
    }
    Elf64_Manager* manager = load_elf64_file(argv[1]);
    printf("Loaded file\n");

    // manager->e_hdr.e_shnum = 0;
    // manager->e_hdr.e_shoff = 0;


    print_all_elf64_program_header(manager);
    print_all_elf64_section_header(manager);

    write_elf64_file(manager, argv[1]);

    free_manager64(manager);

    return 0;
}