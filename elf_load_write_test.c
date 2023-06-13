#include "elf_support.h"
#include <stdio.h>

int main(int argc, char** argv){
    if(argc < 2){
        printf("Need to specify a path to a file as an argument\n");
        return 1;
    }
    Elf64_Manager* manager = load_elf64_file(argv[1]);
    printf("Loaded file\n");

    print_all_elf64_program_header(manager);
    print_all_elf64_section_header(manager);

    int index = get_first_section_index_by_name(manager, ".text");
    if(index != -1){
        print_elf64_section_header(manager,index);
    }


    write_elf64_file(manager, argv[1]);
    free_manager64(manager);
    printf("File should be unchanged\n");
    return 0;
}