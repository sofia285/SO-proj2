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
#define MESSAGE_SIZE 1029
#define SCANF_SIZE 1024
#define PIPE_NAME_SIZE 256
#define BOX_NAME_SIZE 32
#define CODE_SIZE 1
#define LIST_MESSAGE_SIZE 58


int main(int argc, char **argv) {
   int fserv, fman;
   ssize_t n;
   uint8_t code = 3;
   size_t register_info_offset = 0;
   void *message = malloc(MESSAGE_SIZE*sizeof(uint8_t));
   void *register_info = malloc(REGISTER_INFO_SIZE*sizeof(uint8_t));

   //verificatins
   if ((argc < 4) || (argc > 5)) {exit(1);}

   // opens the <register_pipe_name> to talk with the server
   if ((fserv = open (argv[1], O_WRONLY)) < 0) {exit(1);}

   // creates manager's register request
   if (argc == 5) {

      if (!strcmp(argv[3], "create")) {
         code = 3;
      }

      else if (!strcmp(argv[3], "remove")){
         code = 5;
      }

      memcpy(register_info, &code, CODE_SIZE); 
      register_info_offset += CODE_SIZE;
      memcpy(register_info + register_info_offset, argv[2], PIPE_NAME_SIZE);
      register_info_offset += PIPE_NAME_SIZE;
      memcpy(register_info + register_info_offset, argv[4], BOX_NAME_SIZE);
   }
   else if (!strcmp(argv[3], "list")){
      code = 7;
      memcpy(register_info, &code, CODE_SIZE); 
      register_info_offset += CODE_SIZE;
      memcpy(register_info + register_info_offset, argv[2], PIPE_NAME_SIZE);
      register_info_offset += PIPE_NAME_SIZE;
   }

   // sends the OP_CODE, <pipe_name> and <box_name> (if there is one) to the server
   n = write(fserv, register_info, REGISTER_INFO_SIZE);
   if (n <= 0) {exit(1);}

   // creates the fifo <pipe_name>
   unlink(argv[2]);
   if (mkfifo(argv[3], 0777) < 0) {exit (1);}

   // opens the <pipe_name>
   if ((fman = open (argv[2], O_RDONLY)) < 0) {exit(1);}

   // receives the message(s) from the server through the <pipe_name>
   if (argc == 5) {
      n = read(fman, message, MESSAGE_SIZE);
   }
   else {
      n = read(fman, message, LIST_MESSAGE_SIZE);
   }

   /* closes the pipe */
   sleep(1);
   close(fman);
   return 0;
}
