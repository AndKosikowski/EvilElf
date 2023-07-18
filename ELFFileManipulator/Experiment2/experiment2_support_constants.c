#include <stdio.h>
#include <stdlib.h>
#include "../elf_support.h"
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>


int main(int argc, char** argv){
    if(argc < 2){
        printf("Need to specify an ELF file to modify\n");
        return 1;
    }

    Elf_Manager* malware = load_elf_file(argv[1]);

    char buffer[strlen(argv[1])+40];

    Elf_Xword added_size = 0;


    int size = get_file_name_size_from_path(malware->file_path);
    char base_malware_file[200];
    strcpy(base_malware_file, malware->file_path + strlen(malware->file_path)-size);

    free_manager(malware);
    
    uint8_t value = 56;
    for(int i = 0; i < 200; i++){
        malware = load_elf_file(argv[1]);
        strcpy(buffer,base_malware_file);
        strcat(buffer,"_append__50k_");
        char buffer2[4];
        snprintf(buffer2,4, "%d",value);
        strcat(buffer,buffer2);

        int new_section_index = append_new_section(malware, 50000);

        uint8_t* file_section = malware->file_sections[new_section_index];
        Elf_Xword section_size = malware->s_hdr[new_section_index].sh_size;

        for(int i = 0; i < section_size; i++){
            file_section[i] = value;
        }
        write_elf_file(malware, buffer);
        free_manager(malware);
        value++;
    }

    uint8_t value = 56;
    for(int i = 0; i < 200; i++){
        malware = load_elf_file(argv[1]);
        strcpy(buffer,base_malware_file);
        strcat(buffer,"_append__500k_");
        char buffer2[4];
        snprintf(buffer2,4, "%d",value);
        strcat(buffer,buffer2);

        int new_section_index = append_new_section(malware, 500000);

        uint8_t* file_section = malware->file_sections[new_section_index];
        Elf_Xword section_size = malware->s_hdr[new_section_index].sh_size;

        for(int i = 0; i < section_size; i++){
            file_section[i] = value;
        }
        write_elf_file(malware, buffer);
        free_manager(malware);
        value++;
    }

    return 0;
}