    #include <elf.h>
    #include <stdio.h>
    #include <stdlib.h>
     
    int main()
    {
      FILE* fp = fopen("/home/danielcho/Desktop/NSF-REU/ELFFileManipulator/VSCodeStuff/example", "r+b");
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

      // printf("%d\n",hdr.e_version);

      // Elf64_Phdr p[hdr.e_phnum];




      // printf("ELF e_ident padding: \n");
      // for (int i = 9; i < 16; i++)
      // {
      //    printf("%d\n", hdr.e_ident[i]);
      // }

      // for (int i = 9; i < 16; i++)
      // {
      //    hdr.e_ident[i] = 0; //changing the first byte of padding in e_ident in ELF header, original is 0
      // }

      // // hdr.e_ident[9] = 1;

      // printf("ELF e_ident padding after: \n");
      // for (int i = 9; i < 16; i++)
      // {
      //    printf("%d\n", hdr.e_ident[i]);
      // }

      printf("ELF e_ident size before: \n");


      rewind(fp);
      fwrite(&hdr, sizeof(hdr), 1, fp);
      fclose(fp);
     
       return 0;
    }