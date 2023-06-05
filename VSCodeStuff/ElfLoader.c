#include <elf.h>
#include <stdio.h>
#include <stdlib.h>

//https://stackoverflow.com/questions/25246236/read-file-into-struct






int main(){
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

    printf("%d\n",hdr.e_version);
//https://refspecs.linuxbase.org/elf/elf.pdf
    Elf64_Phdr p[hdr.e_phnum];


    printf("ELF e_ident padding: \n");
    for (int i = 9; i < 16; i++)
    {
        printf("%d\n", hdr.e_ident[i]);
    }


    for(int i = 0; i < hdr.e_phnum; i++){
        p[i];
        if(1 != fread(&p[i], sizeof(hdr), 1, fp)){
            printf("failed to read elf header\n");
            exit(1);
        }

    }


    // printf("ELF e_ident padding after: \n");
    // for (int i = 9; i < 16; i++)
    // {
    //     printf("%d\n", hdr.e_ident[i]);
    // }

    // rewind(fp);
    // fwrite(&hdr, sizeof(hdr), 1, fp);
    fclose(fp);

    // If program doesn't exit, header was read and can be worked with down here.
}
