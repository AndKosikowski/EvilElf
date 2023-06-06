#include <elf.h>
#include <stdlib.h>

typedef struct elf64_manager {
    Elf64_Ehdr e_hdr;
    Elf64_Phdr* p_hdr;
    Elf64_Shdr* s_hdr;
} Elf64_Manager;

Elf64_Manager* initialize_manager64(int num_phdr, int num_shdr){
    Elf64_Manager* manager = (Elf64_Manager*) malloc(sizeof(Elf64_Manager));
    manager->p_hdr = (Elf64_Phdr*) malloc(sizeof(Elf64_Phdr)* num_phdr); 
    manager->s_hdr = (Elf64_Shdr*) malloc(sizeof(Elf64_Shdr)* num_shdr); 
    return manager;
}

void free_manager64(Elf64_Manager* manager){
    free(manager->p_hdr);
    free(manager->s_hdr);
    free(manager);
}

typedef struct elf32_manager {
    Elf32_Ehdr e_hdr;
    Elf32_Phdr* p_hdr;
    Elf32_Shdr* s_hdr;
} Elf32_Manager;

Elf32_Manager* initialize_manager32(int num_phdr, int num_shdr){
    Elf32_Manager* manager = (Elf32_Manager*) malloc(sizeof(Elf32_Manager));
    manager->p_hdr = (Elf32_Phdr*) malloc(sizeof(Elf32_Phdr)* num_phdr); 
    manager->s_hdr = (Elf32_Shdr*) malloc(sizeof(Elf32_Shdr)* num_shdr); 
    return manager;
}

void free_manager32(Elf32_Manager* manager){
    free(manager->p_hdr);
    free(manager->s_hdr);
    free(manager);
}