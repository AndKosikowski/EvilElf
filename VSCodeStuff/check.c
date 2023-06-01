#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <elf.h>

int main() {
    FILE *file = fopen("/home/danielcho/Desktop/NSF-REU/ELFFileManipulator/VSCodeStuff/target.c", "rb");
    if (file == NULL) {
        printf("Failed to open the file.\n");
        return 1;
    }

    Elf64_Ehdr elfHeader;
    fread(&elfHeader, sizeof(elfHeader), 1, file);

    // Verify the changes
    if (elfHeader.e_ident[9] == 99) {
        printf("The EI_CLASS field was changed.\n");
    } else {
        printf("The EI_CLASS field was not changed.\n");
    }

    fclose(file);

    return 0;
}