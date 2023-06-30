#include <stdio.h>
#include <stdlib.h>
#include "elf_support.h"
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>


int main(int argc, char** argv){
    if(argc < 4){
        printf("Need to specify two paths to different files and a value between 0-255\n");
        return 1;
    }

    long arg = strtol(argv[3], NULL, 10);
    int loop_count = 1;
    if(arg == -1){
        printf("Creating all 0-255 value versions\n");
        loop_count = 256;
        arg = 0;
    }
    else if(arg < 0 || arg > 255){
        printf("Value needs to be between 0-255 inclusive \n");
        return 2;
    }

    Elf64_Manager* malware = load_elf64_file(argv[1]);
    Elf64_Manager* benign = load_elf64_file(argv[2]);

    uint8_t user_value = arg;
    int text_section_index = get_next_section_index_by_name(benign,".text",0);
    Elf64_Xword text_section_size = benign->s_hdr[text_section_index].sh_size;

    char buffer[strlen(argv[1])+40];
    uint8_t user_loop_value = user_value;
    for(int i = 0; i < loop_count; i++){
        
        //change e_flags and e_ident in ELF header
        change_elf_header(malware, user_loop_value, buffer, argv[1]);

        //inserting 4's to new section at end of malware file
        malware = load_elf64_file(argv[1]);
        append_value(malware, user_loop_value, buffer, argv[1]);

        //changing the gaps between segments/sections
        malware = load_elf64_file(argv[1]);
        int* gap_start;
        int* gap_size;
        int gap_count = 0;
        find_gaps_in_elf64_file(malware, &gap_start,&gap_size,&gap_count);

        char* folder = "ModifiedElfOutput/"; 

        int size = strlen(malware->file_path);
        char output_path[18+size+6+4];
        strcpy(output_path, folder);
        strcat(output_path, malware->file_path);
        strcat(output_path,"_gaps_");
        char buffer2[4];
        snprintf(buffer2,4,"%d",user_loop_value);
        strcat(output_path,buffer2);

        write_elf64_file(malware, output_path+18);

        FILE* fp = fopen(output_path, "r+b");
        if(fp == NULL){
            printf("Output path was unable to be opened to fill gaps\n");
            return 0;
        }

        for(int i = 0; i < gap_count; i++){
            fseek(fp,gap_start[i], SEEK_SET);
            for(int j = 0; j < gap_size[i]; j++){
                uint8_t k = user_value;
                fwrite(&k,1,1,fp);
            }
        }

        fclose(fp);
        free(gap_size);
        free(gap_start);

        user_loop_value = user_loop_value + 1;
    }

    //inserting benign text section to new section at end of malware file
    malware = load_elf64_file(argv[1]);
    append_benign_x1(malware, benign, text_section_index, text_section_size, buffer, argv[1]);

    //inserting benign text section ten times
    malware = load_elf64_file(argv[1]);
    append_benign_x10(malware, benign, text_section_index, text_section_size, buffer, argv[1]);

    //extending dynamic segment and inserting benign text section
    malware = load_elf64_file(argv[1]);
    write_extended_dynamic(malware, benign, text_section_index, text_section_size, buffer, argv[1]);

    //changing note, comment, debug sections if the exist
    malware = load_elf64_file(argv[1]);
    change_note_comment_debug(malware, benign, text_section_index, buffer, argv[1]);


    

    free_manager64(benign);
    return 0;
}