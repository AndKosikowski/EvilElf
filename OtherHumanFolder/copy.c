#include "../elf_support.c"
#include <stdio.h>
#include <unistd.h>
#define ADDENDUM 1024
#define PAGE_SIZE 4096


void print_segment_virtual_addresses(Elf64_Manager* manager) {
    for(int i = 0; i < manager->e_hdr.e_phnum; i++) {
        printf("%i: %lx\n", i, manager->p_hdr[i].p_vaddr);
    }
}


// void extend_load_segment(Elf64_Manager* manager, struct seg_sect* seg_table, FILE* fp) {
//     int cutoff;
//     int file_size;
//     int count = 0;
//     for (int i = 0; i < manager->e_hdr.e_phnum; i++) {
//         Elf64_Phdr segment = manager->p_hdr[i];
//         if (segment.p_type == 1) { //will need to change this for PT_LOAD segment
//             count++;
//             printf("%i\n", count);
//             printf("Segment %i of offset %lx and size %lx\n", i, segment.p_offset, segment.p_filesz);
//         }
//         if (count == 2) {
//             cutoff = segment.p_offset;
//             file_size = segment.p_filesz;
//             manager->p_hdr[i].p_filesz += ADDENDUM; //remove this until later until you find the last PT_LOAD segment
//             break;
//         }
//     }
//     printf("%x, %x", cutoff, file_size);
//     int dynam_section;
//     for (int i = 0; i < manager->e_hdr.e_shnum; i++) {
//         Elf64_Shdr section = manager->s_hdr[i];
//         if (section.sh_offset >= cutoff && section.sh_offset < cutoff + file_size) { //going to need to change this part (maybe) in case there is multiple sections in the segment, and so make an array, append all of the sections that satisfy this condition, and choose the last one's offset + file_size as the place to insert new bytes
//             printf("%lx\n", section.sh_offset);
//             dynam_section = i;
//         }
//     }

//     void* ptr = realloc(manager->file_sections[dynam_section], sizeof(uint8_t*) * (file_size + ADDENDUM));
//     if (ptr == NULL) {
//         printf("Insufficient memory during realloc\n");
//         return;
//     }
//     manager->file_sections[dynam_section] = ptr;
//     manager->s_hdr[dynam_section].sh_size += ADDENDUM;

//     printf("%x\n", dynam_section);
//     uint8_t* section = manager->file_sections[dynam_section];
//     for (int i = file_size; i < file_size + ADDENDUM; i++) {
//         section[i] = 4;
//     }
//     memcpy(&(manager->file_sections[dynam_section]), &section, sizeof(uint8_t*));

//     for (int i = 0; i < manager->e_hdr.e_phnum; i++) {
//         Elf64_Phdr segment = manager->p_hdr[i];
//         if (segment.p_offset >= cutoff + file_size) {
//             printf("segment");
//             manager->p_hdr[i].p_offset += ADDENDUM;
//         }
//     }
//     for (int i = 0; i < manager->e_hdr.e_shnum; i++) {
//         Elf64_Shdr section = manager->s_hdr[i];
//         if (section.sh_offset >= cutoff + file_size) {
//             printf("yay\n");
//             manager->s_hdr[i].sh_offset += ADDENDUM;
//         }
//     }

//     manager->e_hdr.e_shoff += ADDENDUM;
// }




int main(int argc, char** argv){
    if(argc < 2){
        printf("Need to specify a path to a file as an argument\n");
        return 1;
    }

    // fseek(fp, 0, SEEK_END);
    // long int sz = ftell(fp);
    // fseek(fp, 0, SEEK_SET);
    // printf("%lx\n", sz);
    // long int diff = 102400 - sz;

    // struct seg_sect* seg_table = segment_table(manager);
    // print_segment_virtual_addresses(manager);
    // // print_all_elf64_section_header(manager);
    // extend_dynamic_segment(manager, fp);
    // extend_load_segment(manager, seg_table, fp);

    // printf("%i\n", get_next_program_index_by_type(manager, 4, 0));

    // print_sections_in_segments(manager, fp);
    // print_all_elf64_program_header(manager);
  // char buffer[strlen(argv[1])+40];
    // strcpy(buffer,argv[1]);
    // strcat(buffer,"_fours");
    // write_elf64_file(malware, buffer);
    // free_manager64(malware);

    Elf64_Manager* malware = load_elf64_file(argv[1]);
    Elf64_Manager* benign = load_elf64_file(argv[2]);
    FILE* fp = fopen(malware->file_path, "r+b");

    int* note_section_indexes = change_note(malware);
    int comment_section_index = change_comment(malware);
    int debug_section_index = change_debug(malware);

    int text_section_index = get_next_section_index_by_name(benign,".text",0);
    Elf64_Xword text_section_size = benign->s_hdr[text_section_index].sh_size;
    printf("Text section index: %d size: %ld\n",text_section_index, text_section_size);

    // int dynamic_section_size = return_dynamic_size(malware);
    // int dynamic_section_index = extend_dynamic_segment(malware, text_section_size);

    // uint8_t* file_section = malware->file_sections[dynamic_section_index];

    // memcpy(malware->file_sections[dynamic_section_index] + dynamic_section_size - 1, benign->file_sections[text_section_index], text_section_size);

    char buffer[strlen(argv[1])+40];
    strcpy(buffer,argv[1]);
    strcat(buffer,"_extend_dynamic");
    write_elf64_file(malware, buffer);
    free_manager64(malware);
    free_manager64(benign);


    // write_elf64_file(manager, argv[1]);
    // free_seg_table(manager, seg_table);
    // free_manager64(manager);
    // fclose(fp);
    return 0;
}