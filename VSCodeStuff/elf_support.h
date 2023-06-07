#ifndef ELF_SUPPORT_H
#define ELF_SUPPORT_H
#include <elf.h>

typedef struct elf64_manager {
    Elf64_Ehdr e_hdr;
    Elf64_Phdr* p_hdr;
    Elf64_Shdr* s_hdr;
} Elf64_Manager;

Elf64_Manager* initialize_manager64(int num_phdr, int num_shdr);

void free_manager64(Elf64_Manager* manager);

typedef struct elf32_manager {
    Elf32_Ehdr e_hdr;
    Elf32_Phdr* p_hdr;
    Elf32_Shdr* s_hdr;
} Elf32_Manager;

Elf32_Manager* initialize_manager32(int num_phdr, int num_shdr);

void free_manager32(Elf32_Manager* manager);

int cp(const char *to, const char *from);

Elf64_Manager* load_elf64_file(char* file_path);

int get_file_name_size_from_path(char* file_path);


void write_elf64_file(Elf64_Manager* manager, char* file_path);
#endif