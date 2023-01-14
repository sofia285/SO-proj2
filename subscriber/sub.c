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
  int fsub, fserv;
  char register_info[REGISTER_INFO_SIZE] = {'\0'}, message[MESSAGE_SIZE] = {'\0'};
  ssize_t n;

   // verifications
   if (argc != 5 || strcmp(argv[1], "sub")) {exit(1);}

   // opens the <register_pipe_name> to talk with the server
   if ((fserv = open (argv[2], O_WRONLY)) < 0) {exit(1);}

   // creates subscriber's register request 
   uint8_t code = 2;
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

   // opens the <pipe_name>
   if ((fsub = open (argv[3], O_RDONLY)) < 0) {exit(1);}

   // receives the messages from the server through the <pipe_name>
   n = read(fsub, message, MESSAGE_SIZE);

   /* closes the pipes */
   close(fsub);
   close(fserv);

   return 0;
}
