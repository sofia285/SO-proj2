#include "logging.h"
#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>

#define REGISTER_INFO_SIZE 289
#define MESSAGE_SIZE 1025
#define SCANF_SIZE 1024
#define PIPE_NAME_SIZE 256
#define BOX_NAME_SIZE 32
#define CODE_SIZE 1

int fsub;
int msg_counter = 0;

void sigint_handler(int signum) {
   (void)signum;

   fprintf(stdout,"SIGINT received after %d messages.\n", msg_counter);
   close(fsub);
   exit(0);
}

int main(int argc, char **argv) {

   signal(SIGINT, sigint_handler);

   ssize_t n;
   int fserv;
   uint8_t code = 2;
   size_t register_info_offset = 0;
   void *register_info = malloc(REGISTER_INFO_SIZE*sizeof(uint8_t));

   // verifications
   if (argc != 4) {exit(1);}

   // opens the <register_pipe_name> to talk with the server
   if ((fserv = open (argv[1], O_WRONLY)) < 0) {exit(1);}

   // creates subscriber's register request 
   code = 2;
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

   free(register_info);

   // opens the <pipe_name>
   if ((fsub = open (argv[2], O_RDONLY)) < 0) {exit(1);}

   while(1){
      // receives the messages from the server through the <pipe_name>
      void *message = malloc(MESSAGE_SIZE*sizeof(uint8_t));
      n = read(fsub, message, MESSAGE_SIZE);
      if(n <= 0) break;
      uint8_t second_code;
      char message_txt[SCANF_SIZE];
      memcpy(&second_code, message, CODE_SIZE);
      memcpy(message_txt, message + CODE_SIZE, SCANF_SIZE);

      //Print reply
      fprintf(stdout,"%s\n", message_txt);
      msg_counter++;
      free(message);
   }

   return 0;
}
