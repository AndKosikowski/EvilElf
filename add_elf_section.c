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
    int final_section = 0;
    int final_offset = 0;
    for(int i = 1; i < manager->e_hdr.e_shnum; i++){
        if(manager->s_hdr[i].sh_offset > final_offset){
            // printf("Section offset: %x\n", manager->s_hdr[i].sh_offset);
            final_section = i;
            final_offset = manager->s_hdr[i].sh_offset;
        }
    }

    printf("Last Section: %x - %lx\n",final_offset, final_offset + manager->s_hdr[final_section].sh_size);

    void* ptr = realloc(manager->s_hdr, sizeof(Elf64_Shdr) * (manager->e_hdr.e_shnum + 1));
    if(ptr == NULL){
        printf("Realloc failed when adding section header entry\n");
        return 1;
    }
    manager->s_hdr = ptr;

    Elf64_Shdr new_section; //Point to new last section
    memcpy(&new_section, &(manager->s_hdr[final_section]), sizeof(Elf64_Shdr)); //Copy old last section into it
    manager->e_hdr.e_shnum += 1;
    new_section.sh_offset = (final_offset + new_section.sh_size + 7) & (-8); //Get next multiple of 8 at end of last section
    new_section.sh_size = 1024*1024;  

    manager->e_hdr.e_shoff += new_section.sh_size;

    int new_section_index = manager->e_hdr.e_shnum-1;

    memcpy(&(manager->s_hdr[new_section_index]),&new_section,sizeof(Elf64_Shdr));

    ptr = realloc(manager->file_sections, sizeof(uint8_t*) * manager->e_hdr.e_shnum);
    if(ptr == NULL){
        printf("Realloc failed when adding file section\n");
        return 1;
    }
    ptr = realloc(manager->file_sections[new_section_index], sizeof(uint8_t*) * manager->s_hdr[new_section_index].sh_size);
    if(ptr == NULL){
        printf("Realloc failed when adding file section\n");
        return 1;
    }
    manager->file_sections[new_section_index] = ptr;

    uint8_t* file_section = manager->file_sections[new_section_index];
    for(int i = 0; i < new_section.sh_size; i++){
        file_section[i] = 4;
    }


    write_elf64_file(manager, argv[1]);
    free_manager64(manager);
    return 0;
}