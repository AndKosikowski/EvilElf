#include <stdio.h>
#include <stdlib.h>
#include "elf_support.h"
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

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

int cp(const char *to, const char *from){
    int fd_to, fd_from;
    char buf[4096];
    ssize_t nread;
    int saved_errno;

    fd_from = open(from, O_RDONLY);
    if (fd_from < 0){
        return -1;
    }
        

    fd_to = open(to, O_RDWR | O_CREAT | O_EXCL, 0666);
    if (fd_to < 0){
        goto out_error;
    }
        

    while (nread = read(fd_from, buf, sizeof buf), nread > 0){
        char *out_ptr = buf;
        ssize_t nwritten;

        do {
            nwritten = write(fd_to, out_ptr, nread);
            if (nwritten >= 0){
                nread -= nwritten;
                out_ptr += nwritten;
            }
            else if (errno != EINTR){
                goto out_error;
            }
        } while (nread > 0);
    }

    if (nread == 0){
        if (close(fd_to) < 0){
            fd_to = -1;
            goto out_error;
        }
        close(fd_from);

        /* Success! */
        return 0;
    }

  out_error:
    saved_errno = errno;

    close(fd_from);
    if (fd_to >= 0)
        close(fd_to);

    errno = saved_errno;
    return -1;
}




//https://stackoverflow.com/questions/25246236/read-file-into-struct
Elf64_Manager* load_elf64_file(char* file_path){

    FILE* fp = fopen(file_path, "r+b");
    if(fp == NULL)
    {
        printf("failed to load (likely invalid file path)\n");
        exit(1);
    }

    Elf64_Ehdr hdr;
    if (1 != fread(&hdr, sizeof(hdr), 1, fp))
    {
        printf("failed to read elf header\n");
        exit(1);
    }

    Elf64_Manager* manager = initialize_manager64(hdr.e_phnum,hdr.e_shnum);
    strncpy(manager->file_path,file_path,4095);
    manager->file_path[4096] = '\0';
    memcpy(&(manager->e_hdr), &hdr, sizeof(Elf64_Ehdr));

    printf("Number of program headers: %d\nNumber of section headers: %d\n",hdr.e_phnum,hdr.e_shnum);
    printf("Program Header Offset: %lx, size %x\n",hdr.e_phoff, hdr.e_phentsize*hdr.e_phnum);
    printf("Section Header Offset: %lx, size %x\n",hdr.e_shoff, hdr.e_shentsize*hdr.e_shnum);

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
    get_section_flag(buffer,shdr.sh_type);
    printf("sh_flags: %ld %s\n", shdr.sh_flags,buffer);
    printf("sh_addr: %lx\n", shdr.sh_addr);
    printf("sh_off: %lx\n", shdr.sh_offset);
    printf("sh_size: %lx\n", shdr.sh_size);
    printf("sh_link: %x\n", shdr.sh_link);
    printf("sh_info: %x\n", shdr.sh_info);
    printf("sh_addralign: %lx\n", shdr.sh_addralign);
    printf("sh_entsize: %lx\n\n", shdr.sh_entsize);
}

void get_section_flag(char* string, uint32_t value){
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
    return;
}

void write_elf64_file(Elf64_Manager* manager, char* file_path){
    FILE* fp = fopen(file_path, "r+b");
    char* folder = "ModifiedElfOutput/";
    int size = get_file_name_size_from_path(file_path);
    char output_path[19+size];
    strcpy(output_path, folder);

    strcat(output_path, file_path+strlen(file_path)-size);

    printf("made output_path %s\n",output_path);
    cp(output_path,file_path);
    fclose(fp);
    fp = fopen(output_path,"r+b");

    fwrite(&(manager->e_hdr), sizeof(manager->e_hdr), 1, fp);

    fseek(fp, manager->e_hdr.e_phoff, SEEK_SET);
    for(int i = 0; i < manager->e_hdr.e_phnum; i++){
        fwrite(&(manager->p_hdr[i]), sizeof(Elf64_Phdr),1,fp);
    }
    

    fseek(fp, manager->e_hdr.e_shoff, SEEK_SET);
    for(int i = 0; i < manager->e_hdr.e_phnum; i++){
        fwrite(&(manager->s_hdr[i]), sizeof(Elf64_Shdr),1,fp);
    }

    fclose(fp);
}