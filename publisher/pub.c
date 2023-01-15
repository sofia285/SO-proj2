/*Começar por se ligar ao FIFO do servidor (como escrita) <register_pipe_name>
Manda mensagem ao server pelo fifo: Register info
        -tipo de cliente (pub, sub, manager) aka code. é um integer
        -nome do named pipe: <pipe_name> 256 chars
        -nome da caixa de mensagens: <box_name> 32 chars
Se a ligacao for aceite:
        -Cria o fifo <pipe_name>
        -Lê do terminal
        -Publica a mensagem através do <pipe_name> 

*/

//#include "logging.h"

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
#define SCANF_SIZE 1024
#define PIPE_NAME_SIZE 256
#define BOX_NAME_SIZE 32
#define CODE_SIZE 1

int main(int argc, char **argv) {
   ssize_t n;
   int fpub, fserv;
   uint8_t code = 1;
   size_t register_info_offset = 0;
   void *message = malloc(MESSAGE_SIZE*sizeof(uint8_t));
   void *register_info = malloc(REGISTER_INFO_SIZE*sizeof(uint8_t));

   // verifications
   if (argc != 4) {exit(1);}
   
   // opens the <register_pipe_name> to talk with the server
   if ((fserv = open (argv[1], O_WRONLY)) < 0) {exit(1);}
   
   // creates publisher's register request
   code = 1;
   memcpy(register_info, &code, CODE_SIZE); 
   register_info_offset += CODE_SIZE;
   memcpy(register_info + register_info_offset, argv[2], PIPE_NAME_SIZE);
   register_info_offset += PIPE_NAME_SIZE;
   memcpy(register_info + register_info_offset, argv[3], BOX_NAME_SIZE);

   // sends the OP_CODE, <pipe_name> and <box_name> to the server
   n = write(fserv, register_info, REGISTER_INFO_SIZE);
   if (n <= 0) {exit(1);}

   // creates the fifo <pipe_name>
   unlink(argv[2]);
   if (mkfifo(argv[2], 0777) < 0) {
      exit (1);
   }

   // opens the <pipe_name>
   if ((fpub = open (argv[2], O_WRONLY)) < 0) {
	   exit(1);
   }

   // reads message from terminal
   char buf[SCANF_SIZE];
   int error = scanf("%s", buf);
   if(error <= 0) {exit(1);}
   /*if(fgets(buf, SCANF_SIZE, stdin) == NULL){
      printf("Error scaning message \n");
      exit(1);
   }*/
   code = 9;
   memcpy(message, &code, CODE_SIZE); 
   memcpy(message + CODE_SIZE, &buf, SCANF_SIZE);

   // sends the message to the server through the <pipe_name>
   n = write(fpub, message, MESSAGE_SIZE);

   // if the publisher receives an EOF, it closes the <pipe_name>
   if (!strcmp(message, "EOF")) {close(fpub);}

   /* closes the pipe */
   sleep(1);
   close(fpub);

   return 0;
}

