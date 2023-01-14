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

int main(int argc, char **argv) {
   ssize_t n;
   int fpub, fserv;
   char message[MESSAGE_SIZE] = {'\0'}, register_info[REGISTER_INFO_SIZE] = {'\0'};

   // verifications
   if (argc != 5 || strcmp(argv[1], "pub")) {
      exit(1);
   }
   
   // opens the <register_pipe_name> to talk with the server
   if ((fserv = open (argv[2], O_WRONLY)) < 0) {exit(1);}
   
   // creates publisher's register request 
   uint8_t code = 1;
   register_info[0] = code; //might not be recommended but are both 1 byte long
   strcpy(&register_info[1], argv[3]);
   strcpy(&register_info[33], argv[4]);

   // sends the OP_CODE, <pipe_name> and <box_name> to the server
   n = write(fserv, register_info, REGISTER_INFO_SIZE);
   if (n <= 0) {exit(1);}

   unlink(argv[3]);

   // creates the fifo <pipe_name>
   if (mkfifo(argv[3], 0777) < 0) {
      exit (1);
   }

   // reads message from terminal
   char buf[1024];
   scanf("%s", buf);

   code = 9;
   message[0] = code;
   strcpy(&message[1], buf);

   // if the publisher receives an EOF, it closes the <pipe_name>
   if (!strcmp(message, "EOF")){
      close(fpub);
   }

   // opens the <pipe_name>
   if ((fpub = open (argv[3], O_WRONLY)) < 0) {
	   exit(1);
   }

   // sends the message to the server through the <pipe_name>
   n = write(fpub, message, MESSAGE_SIZE);

   /* closes the pipes */
   close(fpub);
   close(fserv);

   return 0;
}

