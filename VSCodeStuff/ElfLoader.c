#include <elf.h>
#include <stdio.h>
#include <stdlib.h>

//https://stackoverflow.com/questions/25246236/read-file-into-struct






int main(){
    FILE* fp = fopen("/home/danielcho/Desktop/NSF-REU/ELFFileManipulator/VSCodeStuff/example", "rb");
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

    Elf64_Phdr p[hdr.e_phnum];



    // If program doesn't exit, header was read and can be worked with down here.
}
