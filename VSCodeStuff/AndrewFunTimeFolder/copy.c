#include "../elf_support.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]){
   if(argc < 2){
      printf("Need to specify a path to a file as an argument\n");
      return 1;
   }
   Elf64_Manager* manager = load_elf64_file(argv[1]);
   printf("Loaded file\n");

   for(int i = 0; i < manager->e_hdr.e_shnum; i++){
      print_elf64_section_header(manager, i);
   }
   

//  write_elf64_file(manager, argv[1]);
   free_manager64(manager);
   printf("File should be unchanged\n");
   return 0;
}
