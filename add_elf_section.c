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

    //https://stackoverflow.com/questions/2022179/c-quick-calculation-of-next-multiple-of-4
    //Adding section at next multiple of 4/8/16/32/64 might be needed later

    manager->s_hdr = realloc(manager->s_hdr, sizeof(Elf64_Shdr) * (manager->e_hdr.e_shnum + 1));
    if(manager->s_hdr == NULL){
        printf("Realloc space lacking for realloc\n");
        return 1;
    }

    Elf64_Shdr new_section; //Point to new last section
    memcpy(&new_section, &(manager->s_hdr[final_section]), sizeof(Elf64_Shdr)); //Copy old last section into it
    manager->e_hdr.e_shnum += 1;
    new_section.sh_offset = (final_offset + new_section.sh_size + 7) & (-8); //Get next multiple of 8 at end of last section
    new_section.sh_size = 1024;  

    manager->e_hdr.e_shoff += new_section.sh_size;

    memcpy(&(manager->s_hdr[manager->e_hdr.e_shnum-1]),&new_section,sizeof(Elf64_Shdr));

    write_elf64_file(manager, argv[1]);

    char* folder = "ModifiedElfOutput/";
    int size = get_file_name_size_from_path(argv[1]);
    char output_path[19+size];
    strcpy(output_path, folder);
    strcat(output_path, argv[1]+strlen(argv[1])-size);

    FILE* fp = fopen(output_path, "r+b");
    fseek(fp, new_section.sh_offset,SEEK_SET);

    for(int i = 0; i < new_section.sh_size; i++){
        uint8_t j = 4;
        fwrite(&j, sizeof(j), 1, fp);
    }

    free_manager64(manager);
    fclose(fp);
    return 0;
}