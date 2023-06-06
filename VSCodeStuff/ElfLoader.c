#include <elf.h>
#include <stdio.h>
#include <stdlib.h>
#include "elf_support.h"
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

//https://stackoverflow.com/a/2180788
int cp(const char *to, const char *from){
    int fd_to, fd_from;
    char buf[4096];
    ssize_t nread;
    int saved_errno;

    fd_from = open(from, O_RDONLY);
    if (fd_from < 0){
        return -1;
    }
        

    fd_to = open(to, O_WRONLY | O_CREAT | O_EXCL, 0666);
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

    FILE* fp = fopen("example", "r+b");
    if(fp == NULL)
    {
        printf("failed to load\n");
        exit(1);
    }

    Elf64_Ehdr hdr;
    if (1 != fread(&hdr, sizeof(hdr), 1, fp))
    {
        printf("failed to read elf header\n");
        exit(1);
    }

    Elf64_Manager* manager = initialize_manager64(hdr.e_phnum,hdr.e_shnum);
    manager->e_hdr = hdr;


    printf("Number of program headers: %d\nNumber of section headers: %d\n",hdr.e_phnum,hdr.e_shnum);

    printf("ELF e_ident padding: \n");
    for (int i = 9; i < 16; i++)
    {
        printf("%d\n", manager->e_hdr.e_ident[i]);
    }

    fseek(fp,hdr.e_phoff ,SEEK_SET);

    for(int i = 0; i < hdr.e_phnum; i++){
        if(1 != fread(&(manager->p_hdr[i]), sizeof(manager->p_hdr[i]), 1, fp)){
            printf("failed to read program header\n");
            exit(1);
        }
    }

    fseek(fp,hdr.e_shoff ,SEEK_SET);

    for(int i = 0; i < hdr.e_shnum; i++){
        if(1 != fread(&(manager->s_hdr[i]), sizeof(manager->s_hdr[i]), 1, fp)){
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
    fwrite(manager->p_hdr, sizeof(manager->p_hdr),1,fp);
    fseek(fp, manager->e_hdr.e_shoff, SEEK_SET);
    fwrite(manager->s_hdr, sizeof(manager->s_hdr),1,fp);
    fclose(fp);
}

int main(){
    Elf64_Manager* manager = load_elf64_file("example");
    printf("Loaded file\n");
    write_elf64_file(manager, "example");
    free_manager64(manager);
    return 0;
}