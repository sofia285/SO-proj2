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
   int fsub, fserv;
   uint8_t code = 2;
   size_t register_info_offset = 0;
   void *message = malloc(MESSAGE_SIZE*sizeof(uint8_t));
   void *register_info = malloc(REGISTER_INFO_SIZE*sizeof(uint8_t));

   // verifications
   if (argc != 5 || strcmp(argv[1], "sub")) {exit(1);}

   // opens the <register_pipe_name> to talk with the server
   if ((fserv = open (argv[2], O_WRONLY)) < 0) {exit(1);}

   // creates subscriber's register request 
   code = 2;
   memcpy(register_info, &code, CODE_SIZE); 
   register_info_offset += CODE_SIZE;
   memcpy(register_info + register_info_offset, argv[3], PIPE_NAME_SIZE);
   register_info_offset += PIPE_NAME_SIZE;
   memcpy(register_info + register_info_offset, argv[4], BOX_NAME_SIZE);
   
   // sends the OP_CODE, <pipe_name> and <box_name> to the server
   n = write(fserv, register_info, REGISTER_INFO_SIZE);
   if (n <= 0) {exit(1);}

   unlink(argv[3]);

   // creates the fifo <pipe_name>
   if (mkfifo(argv[3], 0777) < 0) {
      exit (1);
   }

   // opens the <pipe_name>
   if ((fsub = open (argv[3], O_RDONLY)) < 0) {exit(1);}

   // receives the messages from the server through the <pipe_name>
   n = read(fsub, message, MESSAGE_SIZE);

   /* closes the pipes */
   close(fsub);
   close(fserv);

   return 0;
}
