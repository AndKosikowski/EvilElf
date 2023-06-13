#include <stdio.h>
#include <stdlib.h>
#include "elf_support.h"
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

Elf64_Manager* initialize_manager64(int num_phdr, int num_shdr){
    Elf64_Manager* manager = (Elf64_Manager*) malloc(sizeof(Elf64_Manager));
    manager->p_hdr = (Elf64_Phdr*) malloc(sizeof(Elf64_Phdr)* num_phdr); 
    manager->s_hdr = (Elf64_Shdr*) malloc(sizeof(Elf64_Shdr)* num_shdr); 
    manager->file_sections = (uint8_t**) malloc(sizeof(uint8_t*) * num_shdr);
    for(int i = 0; i < num_shdr;i++){
        manager->file_sections[i] = (uint8_t*) malloc(0);
    }
    return manager;
}


void free_manager64(Elf64_Manager* manager){
    for(int i = 0; i < manager->e_hdr.e_shnum; i++){
        free(manager->file_sections[i]);
    }
    free(manager->file_sections);
    free(manager->p_hdr);
    free(manager->s_hdr);
    free(manager);
}

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

//Currently takes an ELF file and parses it into a struct of elf headers and tables
// Elf64_Manager* parse_elf64_file_without_saving(char* file_path){

//     FILE* fp = fopen(file_path, "r+b");
//     if(fp == NULL)
//     {
//         printf("failed to load (likely invalid file path)\n");
//         exit(1);
//     }

//     Elf64_Ehdr hdr;
//     if (1 != fread(&hdr, sizeof(hdr), 1, fp))
//     {
//         printf("failed to read elf header\n");
//         exit(1);
//     }

//     Elf64_Manager* manager = initialize_manager64(hdr.e_phnum,hdr.e_shnum);
//     strncpy(manager->file_path,file_path,4095);
//     manager->file_path[4095] = '\0';
//     memcpy(&(manager->e_hdr), &hdr, sizeof(Elf64_Ehdr));

//     printf("Number of program headers: %d\nNumber of section headers: %d\n",hdr.e_phnum,hdr.e_shnum);
//     printf("Program Header Offset: %#lx, size %#x\n",hdr.e_phoff, hdr.e_phentsize*hdr.e_phnum);
//     printf("Section Header Offset: %#lx, size %#x\n",hdr.e_shoff, hdr.e_shentsize*hdr.e_shnum);

//     fseek(fp,hdr.e_phoff ,SEEK_SET);
    
//     for(int i = 0; i < hdr.e_phnum; i++){
//         if(1 != fread(&(manager->p_hdr[i]), sizeof(Elf64_Phdr), 1, fp)){
//             printf("failed to read program header\n");
//             exit(1);
//         }
//     }

//     fseek(fp,hdr.e_shoff ,SEEK_SET);

//     for(int i = 0; i < hdr.e_shnum; i++){
//         if(1 != fread(&(manager->s_hdr[i]), sizeof(Elf64_Shdr), 1, fp)){
//             printf("failed to read section header\n");
//             exit(1);
//         }
//     }


//     fclose(fp);

//     return manager;
// }

void load_elf64_file_sections(Elf64_Manager* manager){
    FILE* fp = fopen(manager->file_path, "r+b"); 
    printf("Loading sections\n");
    for(int i = 0; i < manager->e_hdr.e_shnum; i++){
        Elf64_Off offset = manager->s_hdr[i].sh_offset;
        fseek(fp, offset, SEEK_SET);
        uint8_t* ptr = (uint8_t*) realloc(manager->file_sections[i],manager->s_hdr[i].sh_size);
        if(manager->s_hdr[i].sh_size == 0 && ptr == NULL && errno == ENOMEM){
            printf("Realloc failed because section %d is too large \n", i); // Could write to file instead to prevent this
            fclose(fp);
            return;
        }
        manager->file_sections[i] = ptr;
        fread(manager->file_sections[i], manager->s_hdr[i].sh_size, 1, fp);
    }
    fclose(fp);
}

//Currently takes an ELF file and parses it into a struct of elf headers and tables
Elf64_Manager* load_elf64_file(char* file_path){

    FILE* fp = fopen(file_path, "r+b");
    if(fp == NULL){
        printf("failed to load (likely invalid file path)\n");
        exit(1);
    }

    Elf64_Ehdr hdr;
    if (1 != fread(&hdr, sizeof(hdr), 1, fp)){
        printf("failed to read elf header\n");
        exit(1);
    }

    Elf64_Manager* manager = initialize_manager64(hdr.e_phnum,hdr.e_shnum);
    strncpy(manager->file_path,file_path,4095);
    manager->file_path[4095] = '\0';
    memcpy(&(manager->e_hdr), &hdr, sizeof(Elf64_Ehdr));

    printf("Number of program headers: %d\nNumber of section headers: %d\n",hdr.e_phnum,hdr.e_shnum);
    printf("Program Header Offset: %#lx, size %#x\n",hdr.e_phoff, hdr.e_phentsize*hdr.e_phnum);
    printf("Section Header Offset: %#lx, size %#x\n",hdr.e_shoff, hdr.e_shentsize*hdr.e_shnum);

    fseek(fp,hdr.e_phoff ,SEEK_SET);
    
    for(int i = 0; i < hdr.e_phnum; i++){
        if(1 != fread(&(manager->p_hdr[i]), sizeof(Elf64_Phdr), 1, fp)){
            printf("failed to read program header\n");
            exit(1);
        }
    }

    fseek(fp,hdr.e_shoff ,SEEK_SET);

    for(int i = 0; i < hdr.e_shnum; i++){
        if(1 != fread(&(manager->s_hdr[i]), sizeof(Elf64_Shdr), 1, fp)){
            printf("failed to read section header\n");
            exit(1);
        }
    }


    fclose(fp);

    load_elf64_file_sections(manager);

    return manager;
}



int get_file_name_size_from_path(char* file_path){
    int length = strlen(file_path);
    for(int i = length -1; i > 0; i--){
        if(file_path[i] == '/' || file_path[i] == '\\'){
            return length-i+1;
        }
    }
    return length;
}

void print_elf64_program_header(Elf64_Manager* manager, int index){
    Elf64_Phdr phdr = manager->p_hdr[index];

    char buffer[1024];
    printf("Program Index %d\n",index);
    get_program_type(buffer, phdr.p_type);
    printf("p_type: %d %s\n", phdr.p_type, buffer);
    printf("p_offset: %#lx\n", phdr.p_offset);
    printf("p_vaddr: %#lx\n", phdr.p_vaddr);
    printf("p_paddr: %#lx\n", phdr.p_paddr);
    printf("p_filesz: %#lx\n", phdr.p_filesz);
    printf("p_memsz: %#lx\n", phdr.p_memsz);
    get_program_flags(buffer,phdr.p_flags);
    printf("p_flags: %x %s\n", phdr.p_flags,buffer);
    printf("p_align: %#lx\n\n", phdr.p_align);

}

void print_all_elf64_program_header(Elf64_Manager* manager){
    for(int i = 0; i < manager->e_hdr.e_phnum; i++){
        print_elf64_program_header(manager,i);
    }
}

void print_elf64_section_header(Elf64_Manager* manager, int index){
    FILE* fp = fopen(manager->file_path, "r+b");
    Elf64_Shdr shdr = manager->s_hdr[index];
    Elf64_Off string_table_offset = manager->s_hdr[manager->e_hdr.e_shstrndx].sh_offset;
    fseek(fp, string_table_offset + shdr.sh_name,SEEK_SET);

    char buffer[1024];
    fgets(buffer, 1024, fp);
    printf("Section Index %d\n",index);
    printf("sh_name: (offset) %d (entry in string table) %s\n", shdr.sh_name,buffer);
    get_section_type(buffer,shdr.sh_type);
    printf("sh_type: %d %s\n", shdr.sh_type,buffer);
    get_section_flags(buffer,shdr.sh_type);
    printf("sh_flags: %#lx %s\n", shdr.sh_flags,buffer);
    printf("sh_addr: %#lx\n", shdr.sh_addr);
    printf("sh_off: %#lx\n", shdr.sh_offset);
    printf("sh_size: %#lx\n", shdr.sh_size);
    printf("sh_link: %#x\n", shdr.sh_link);
    printf("sh_info: %#x\n", shdr.sh_info);
    printf("sh_addralign: %#lx\n", shdr.sh_addralign);
    printf("sh_entsize: %#lx\n\n", shdr.sh_entsize);
    fclose(fp);
}

void print_all_elf64_section_header(Elf64_Manager* manager){
    for(int i = 0; i < manager->e_hdr.e_shnum; i++){
        print_elf64_section_header(manager,i);
    }
}

void get_program_type(char* string, uint32_t value){
    switch (value) {
        case 0: strcpy(string, "PT_NULL"); break;
        case 1: strcpy(string, "PT_LOAD"); break;
        case 2: strcpy(string, "PT_DYNAMIC"); break;
        case 3: strcpy(string, "PT_INTERP"); break;
        case 4: strcpy(string, "PT_NOTE"); break;
        case 5: strcpy(string, "PT_SHLIB"); break;
        case 6: strcpy(string, "PT_PHDR"); break;
        case 0x70000000: strcpy(string, "PT_LOPROC"); break;
        case 0x7fffffff: strcpy(string, "PT_HIPROC"); break;
        default: strcpy(string, "UNKNOWN"); break;
    }
}

void get_program_flags(char* string, uint32_t value){
    string[0] = '\0';
    if ((value & 0x1) != 0) strcat(string, "PF_X,");
    if ((value & 0x2) != 0) strcat(string, "PF_W,");
    if ((value & 0x4) != 0) strcat(string, "PF_R,");
    if ((value & 0xf0000000) != 0) strcat(string, "PF_MASKPROC,");
    return;
}

void get_section_flags(char* string, uint32_t value){
    string[0] = '\0';
    if ((value & 0x1) != 0) strcat(string, "SHF_WRITE,");
    if ((value & 0x2) != 0) strcat(string, "SHF_ALLOC,");
    if ((value & 0x4) != 0) strcat(string, "SHF_EXECINSTR,");
    if ((value & 0xf0000000) != 0) strcat(string, "SHF_MASKPROC,");
    return;
}

void get_section_type(char* string, uint32_t value){
    switch (value) {
        case 0: strcpy(string, "SHT_NULL"); break;
        case 1: strcpy(string, "SHT_PROGBITS"); break;
        case 2: strcpy(string, "SHT_SYMTAB"); break;
        case 3: strcpy(string, "SHT_STRTAB"); break;
        case 4: strcpy(string, "SHT_RELA"); break;
        case 5: strcpy(string, "SHT_HASH"); break;
        case 6: strcpy(string, "SHT_DYNAMIC"); break;
        case 7: strcpy(string, "SHT_NOTE"); break;
        case 8: strcpy(string, "SHT_NOBITS"); break;
        case 9: strcpy(string, "SHT_REL"); break;
        case 10: strcpy(string, "SHT_SHLIB"); break;
        case 11: strcpy(string, "SHT_DYNSYM"); break;
        case 0x70000000: strcpy(string, "SHT_LOPROC"); break;
        case 0x7fffffff: strcpy(string, "SHT_HIPROC"); break;
        case 0x80000000: strcpy(string, "SHT_LOUSER"); break;
        case 0xffffffff: strcpy(string, "SHT_HIUSER"); break;
        default: strcpy(string, "UNKNOWN"); break;
    }
}

// void write_elf64_headers(Elf64_Manager* manager, char* file_path){
//     FILE* fp = fopen(file_path, "r+b");
//     char* folder = "ModifiedElfOutput/"; 
//     struct stat st = {0}; 
//     if (stat("ModifiedElfOutput", &st) == -1) {//Creates base directory if needed
//         mkdir("ModifiedElfOutput", S_IRWXU | S_IRWXG | S_IRWXO);
//     }
//     int size = get_file_name_size_from_path(file_path);
//     char output_path[19+size];
//     strcpy(output_path, folder);

//     strcat(output_path, file_path+strlen(file_path)-size);

//     printf("made output_path %s\n",output_path);
//     cp(output_path,file_path);
//     fclose(fp);
//     fp = fopen(output_path,"r+b");
//     fchmod(fileno(fp), S_IRWXU | S_IRWXG | S_IRWXO);

//     fwrite(&(manager->e_hdr), sizeof(Elf64_Ehdr), 1, fp);

//     fseek(fp, manager->e_hdr.e_phoff, SEEK_SET);
//     for(int i = 0; i < manager->e_hdr.e_phnum; i++){
//         fwrite(&(manager->p_hdr[i]), sizeof(Elf64_Phdr),1,fp);
//     }
    

//     fseek(fp, manager->e_hdr.e_shoff, SEEK_SET);
//     for(int i = 0; i < manager->e_hdr.e_shnum; i++){
//         fwrite(&(manager->s_hdr[i]), sizeof(Elf64_Shdr),1,fp);
//     }

//     fclose(fp);
// }

//gets index of first section going by name
int get_first_section_index_by_name(Elf64_Manager* manager, char* name){
    int num_sections = manager->e_hdr.e_shnum;
    uint8_t* string_table_file_section = manager->file_sections[manager->e_hdr.e_shstrndx];

    for(int i = 0; i < num_sections; i++){
        char* found = string_table_file_section + manager->s_hdr[i].sh_name;
        if (strcmp(name,found) == 0){
            return i;
        }
    }
    return -1;
}


//Appends new section of section_size to manager, updates ELF header e_shnum, Section table, and file_sections
int append_new_section(Elf64_Manager* manager, int section_size){
    int final_section = 0;
    int final_offset = 0;
    for(int i = 1; i < manager->e_hdr.e_shnum; i++){
        if(manager->s_hdr[i].sh_offset > final_offset){
            final_section = i;
            final_offset = manager->s_hdr[i].sh_offset;
        }
    }

    void* ptr = realloc(manager->s_hdr, sizeof(Elf64_Shdr) * (manager->e_hdr.e_shnum + 1));
    if(ptr == NULL){
        printf("Realloc failed when adding section header entry\n");
        return -1;
    }
    manager->s_hdr = ptr;

    Elf64_Shdr new_section; //Point to new last section
    memcpy(&new_section, &(manager->s_hdr[final_section]), sizeof(Elf64_Shdr)); //Copy old last section into it
    manager->e_hdr.e_shnum += 1;
    new_section.sh_offset = (final_offset + new_section.sh_size + 7) & (-8); //Get next multiple of 8 at end of last section
    new_section.sh_size = section_size;  

    manager->e_hdr.e_shoff += new_section.sh_size;

    int new_section_index = manager->e_hdr.e_shnum-1;

    memcpy(&(manager->s_hdr[new_section_index]),&new_section,sizeof(Elf64_Shdr));

    ptr = realloc(manager->file_sections, sizeof(uint8_t*) * manager->e_hdr.e_shnum);
    if(ptr == NULL){
        printf("Realloc failed when adding file section\n");
        return -1;
    }
    manager->file_sections = ptr;
    ptr = malloc(sizeof(uint8_t) * section_size);
    if(ptr == NULL){
        printf("Realloc failed when adding file section\n");
        return -1;
    }
    manager->file_sections[new_section_index] = ptr;
    return new_section_index;
}

void write_elf64_file(Elf64_Manager* manager, char* file_path){
    char* folder = "ModifiedElfOutput/"; 

    struct stat st = {0}; 
    if (stat("ModifiedElfOutput", &st) == -1) {//Creates base directory if needed
        mkdir("ModifiedElfOutput", S_IRWXU | S_IRWXG | S_IRWXO);
    }
    int size = get_file_name_size_from_path(file_path);
    char output_path[19+size];
    strcpy(output_path, folder);

    strcat(output_path, file_path+strlen(file_path)-size);

    printf("made output_path %s\n",output_path);
    
    FILE* fp;

    fp = fopen(output_path,"w+b");
    if(fp == NULL){
        printf("Output path was unable to be created\n");
        return;
    }
    fchmod(fileno(fp), S_IRWXU | S_IRWXG | S_IRWXO);

    fwrite(&(manager->e_hdr), sizeof(Elf64_Ehdr), 1, fp);

    fseek(fp, manager->e_hdr.e_phoff, SEEK_SET);
    for(int i = 0; i < manager->e_hdr.e_phnum; i++){
        fwrite(&(manager->p_hdr[i]), sizeof(Elf64_Phdr),1,fp);
    }
    

    fseek(fp, manager->e_hdr.e_shoff, SEEK_SET);
    for(int i = 0; i < manager->e_hdr.e_shnum; i++){
        fwrite(&(manager->s_hdr[i]), sizeof(Elf64_Shdr),1,fp);
    }

    for(int i = 0; i < manager->e_hdr.e_shnum; i++){
        Elf64_Off offset = manager->s_hdr[i].sh_offset;
        Elf64_Xword size = manager->s_hdr[i].sh_size;
        fseek(fp, offset, SEEK_SET);
        if(manager->file_sections[i] == NULL){
            continue;
        }
        fwrite(manager->file_sections[i], 1, size, fp);
    }

    fclose(fp);
}