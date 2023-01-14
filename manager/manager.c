/*Começar por se ligar ao FIFO do servidor (como escrita) <register_pipe_name>
Manda mensagem ao server pelo fifo:
        -tipo de cliente (pub, sub, manager)
        -nome do named pipe: <pipe_name>
        -comando a executar (create remove, list)
        -nome da caixa de mensagens: <box_name> (pode nao existir)
Se a ligacao for aceite:
        -Recebe e imprime a mensagem através do <pipe_name> e termina

*/
/*static void print_usage() {
    fprintf(stderr, "usage: \n"
                    "   manager <register_pipe_name> create <box_name>\n"
                    "   manager <register_pipe_name> remove <box_name>\n"
                    "   manager <register_pipe_name> list\n");
}

#include "logging.h"*/
#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <stdint.h>

#define REGISTER_INFO_SIZE 2000
#define MESSAGE_SIZE 1025


int main(int argc, char **argv) {
   int fserv;
   char register_info[REGISTER_INFO_SIZE];
   ssize_t n;

   if ((argc < 5) || (argc > 6)) {exit(1);}

   if ((fserv = open (argv[2], O_WRONLY)) < 0) {exit(1);}

   uint8_t code;
   if (argc == 6) {
      if (!strcmp(argv[4], "create")) {
         code = 1;
         register_info[0] = (uint8_t)code; //might not be recommended but are both 1 byte long
         strcpy(&register_info[1], argv[3]);
         strcpy(&register_info[33], argv[4]);
      }
      else if (!strcmp(argv[4], "remove")){
         code = 5;
         register_info[0] = code;
         strcpy(&register_info[1], argv[3]);
         strcpy(&register_info[33], argv[4]);
      }
   }

   else {
      code = 5;
      register_info[0] = code;
      strcpy(&register_info[1], argv[3]);
   }

   n = write(fserv, register_info, REGISTER_INFO_SIZE);
   if (n <= 0) {exit(1);}

   /* Fechar os pipes */
   close(fserv);
   return 0;
}
