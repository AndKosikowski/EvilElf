#include <stdio.h>
#include <stdlib.h>
#include "../elf_support.h"
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>


int main(int argc, char** argv){
    // if(argc < 4){
    //     printf("Need to specify two paths to different files and a value between 0-255\n");
    //     return 1;
    // }

    // long arg = strtol(argv[3], NULL, 10);
    // int loop_count = 1;
    // if(arg == -1){
    //     printf("Creating all 0-255 value versions\n");
    //     loop_count = 256;
    //     arg = 0;
    // }
    // else if(arg < 0 || arg > 255){
    //     printf("Value needs to be between 0-255 inclusive \n");
    //     return 2;
    // }

    Elf_Manager* malware = load_elf_file(argv[1]);
    Elf_Manager* benign = load_elf_file(argv[2]);

    char buffer[strlen(argv[1])+strlen(argv[2])+40];

    int text_section_index = get_next_section_index_by_name(benign,".text",0);
    Elf_Xword text_section_size = benign->s_hdr[text_section_index].sh_size;

    Elf_Xword added_size = 0;


    int size = get_file_name_size_from_path(malware->file_path);
    char base_malware_file[200];
    strcpy(base_malware_file, malware->file_path + strlen(malware->file_path)-size);
    

    size = get_file_name_size_from_path(benign->file_path);
    char base_benign_file[200];
    strcpy(base_benign_file, benign->file_path + strlen(benign->file_path)-size);


    strcpy(buffer,base_malware_file);
    strcat(buffer,"_append_benign_50k_");
    strcat(buffer,base_benign_file);

    while(added_size < 50000){
        int new_section_index = append_new_section(malware, text_section_size);
        memcpy(malware->file_sections[new_section_index], benign->file_sections[text_section_index], text_section_size); 
        added_size = added_size + text_section_size;   
    }
    write_elf_file(malware, buffer);
    free_manager(malware);

    // Elf_Manager* malware = load_elf_file(argv[1]);
    // strcpy(buffer,argv[1]);
    // strcat(buffer,"_append_4_50k");
    // while(added_size < 50000){
    //     int new_section_index = append_new_section(malware, text_section_size);
    //     memcpy(malware->file_sections[new_section_index], benign->file_sections[text_section_index], text_section_size);        
    // }
    // write_elf_file(malware, buffer);
    // free_manager(malware);
    
    


    // //extending dynamic segment and inserting benign text section
    // malware = load_elf_file(argv[1]);
    // strcpy(buffer,argv[1]);
    // strcat(buffer,"_extend_dynam_benign_50k");
    // extend_dynamic_segment(malware, 50000);


    // //changing note, comment, debug sections if the exist
    // malware = load_elf_file(argv[1]);
    // change_note_comment_debug(malware, benign, text_section_index, buffer, argv[1]);


        

    free_manager(benign);
    return 0;
}