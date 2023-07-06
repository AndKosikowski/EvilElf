#ifndef ELF_SUPPORT_H
#define ELF_SUPPORT_H
#include <elf.h>
#include <stdio.h>

typedef struct elf64_manager {
    Elf64_Ehdr e_hdr;
    Elf64_Phdr* p_hdr;
    Elf64_Shdr* s_hdr;
    char file_path[4096];
    uint8_t** file_sections;
} Elf64_Manager;

Elf64_Manager* initialize_manager64(int num_phdr, int num_shdr);

void print_elf64_header_table_overview(Elf64_Manager* manager);

//gap_start and gap_size will have their size realloacted based on the number of gaps, need to free them
void find_gaps_in_elf64_file(Elf64_Manager* manager, int** gap_start, int** gap_size, int* gap_count, int display);

void free_manager64(Elf64_Manager* manager);

int append_new_section(Elf64_Manager* manager, int section_size);

int get_next_section_index_by_name(Elf64_Manager* manager, char* name, int index);

int get_next_program_index_by_type(Elf64_Manager* manager, int type, int index);

void print_elf64_program_header(Elf64_Manager* manager, int index);

void print_all_elf64_program_header(Elf64_Manager* manager);

void print_elf64_section_header(Elf64_Manager* manager, int index);

void print_all_elf64_section_header(Elf64_Manager* manager);

void get_program_type(char* string, uint32_t value);

void get_program_flags(char* string, uint32_t value);
void get_section_flags(char* string, uint32_t value);

void get_section_type(char* string, uint32_t value);

typedef struct elf32_manager {
    char file_path[4096];
    Elf32_Ehdr e_hdr;
    Elf32_Phdr* p_hdr;
    Elf32_Shdr* s_hdr;
} Elf32_Manager;

Elf32_Manager* initialize_manager32(int num_phdr, int num_shdr);

void free_manager32(Elf32_Manager* manager);

int cp(const char *to, const char *from);

Elf64_Manager* load_elf64_file(char* file_path);
Elf64_Manager* load_elf64_file_v2(char* file_path);

int get_file_name_size_from_path(char* file_path);

void section_in_segment(Elf64_Manager* manager, Elf64_Phdr segment, FILE* fp);
void print_sections_in_segments(Elf64_Manager* manager, FILE* fp);

struct seg_sect* segment_table(Elf64_Manager* manager);
void free_seg_table(Elf64_Manager* manager, struct seg_sect* seg_table);

int* find_note(Elf64_Manager* manager);
int find_comment(Elf64_Manager* manager);
int find_debug(Elf64_Manager* manager);

int return_dynamic_size(Elf64_Manager* manager);
int extend_dynamic_segment(Elf64_Manager* manager, Elf64_Xword ADDENDUM);

void change_elf_header(Elf64_Manager* malware, uint8_t value, char* buffer, char* argv);
void append_value(Elf64_Manager* malware, uint8_t value, char* buffer, char* argv);
void append_benign_x1(Elf64_Manager* malware, Elf64_Manager* benign, int text_section_index, Elf64_Xword text_section_size, char* buffer, char* argv);
void append_benign_x10(Elf64_Manager* malware, Elf64_Manager* benign, int text_section_index, Elf64_Xword text_section_size, char* buffer, char* argv);
void write_extended_dynamic(Elf64_Manager* malware, Elf64_Manager* benign, int text_section_index, Elf64_Xword text_section_size, char* buffer, char* argv);
void change_note_section(Elf64_Manager* malware, Elf64_Manager* benign, int text_section_index);
void change_comment_section(Elf64_Manager* malware, Elf64_Manager* benign, int text_section_index);
void change_debug_section(Elf64_Manager* malware, Elf64_Manager* benign, int text_section_index);
void change_note_comment_debug(Elf64_Manager* malware, Elf64_Manager* benign, int text_section_index, char* buffer, char* argv);


void write_elf64_file(Elf64_Manager* manager, char* file_path);
void write_elf64_file_v2(Elf64_Manager* manager, char* file_path);
#endif