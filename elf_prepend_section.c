#include <stdio.h>
#include <stdlib.h>
#include "elf_support.h"
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>


int main(int argc, char** argv){
    if(argc < 2){
        printf("Need to specify a path to a file as an argument\n");
        return 1;
    }
    Elf64_Manager* manager = load_elf64_file(argv[1]);

    // int phdr_index = get_next_program_index_by_type(manager, PT_PHDR, 0);

    int phnum = manager->e_hdr.e_phnum;

    int how_to_change_phentry[phnum];//0 dont change, 1 update vaddr, 2 needs to be split and update vaddr

    for(int i = 0; i < phnum; i++){
        Elf64_Phdr phdr = manager->p_hdr[i];
        if (phdr.p_offset < manager->e_hdr.e_phoff){
            char buffer[32];
            get_program_type(buffer,phdr.p_type);
            printf("Section %d of type %s begins before program header\n", i, buffer);
            if(phdr.p_offset + phdr.p_filesz >= manager->e_hdr.e_phoff){
                printf("The above section is possibly problematic\n\n");
                how_to_change_phentry[i] = 2;
            }
            else{
                how_to_change_phentry[i] = 0;
            }
        }else{
            how_to_change_phentry[i] = 1;
        }
        
    }



    write_elf64_file(manager, argv[1]);
    free_manager64(manager);
    return 0;
}