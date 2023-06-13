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


void write_elf64_file(Elf64_Manager* manager, char* file_path);
void write_elf64_file_v2(Elf64_Manager* manager, char* file_path);
#endif