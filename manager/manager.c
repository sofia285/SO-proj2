#include "logging.h"
#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <stdint.h>

#define REGISTER_INFO_SIZE 289
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
   if (mkfifo(argv[2], 0777) < 0) {exit (1);}

   // opens the <pipe_name>
   if ((fman = open (argv[2], O_RDONLY)) < 0) {exit(1);}

   // receives the message(s) from the server through the <pipe_name>
   if (argc == 5) {
      n = read(fman, message, MESSAGE_SIZE);
      uint8_t second_code;
      int32_t return_code;
      char error_msg[SCANF_SIZE];
      memcpy(&second_code, message, CODE_SIZE);
      memcpy(&return_code, message + CODE_SIZE, sizeof(int32_t));
      memcpy(error_msg, message + CODE_SIZE + sizeof(int32_t), SCANF_SIZE);

      //Print reply
      if(return_code == 0) fprintf(stdout, "OK\n");
      else fprintf(stdout,"ERROR %s\n", error_msg);
   }
   else {
      void *manager_message = malloc(LIST_MESSAGE_SIZE*sizeof(uint8_t));
      n = read(fman, manager_message, LIST_MESSAGE_SIZE);

      ssize_t offset = 0;
      uint8_t second_code = 8;
      uint8_t last = 0; //1 if it is the last element (otherwise 0) 
      char box_name[BOX_NAME_SIZE];
      uint64_t box_size; //box size (bytes)
      uint64_t n_publishers;
      uint64_t n_subscribers;

      memcpy(&second_code, manager_message, CODE_SIZE); 
      offset+=CODE_SIZE;
      memcpy(&last,manager_message + offset,  CODE_SIZE); 
      offset+=CODE_SIZE;
      memcpy(box_name, manager_message + offset, BOX_NAME_SIZE);
      offset+=BOX_NAME_SIZE;
      memcpy(&box_size, manager_message + offset, sizeof(uint64_t));
      offset+=8;
      memcpy(&n_publishers, manager_message + offset,sizeof(uint64_t));
      offset+=8;         
      memcpy(&n_subscribers, manager_message + offset, sizeof(uint64_t));

      fprintf(stdout, "%s %zu %zu %zu\n", box_name, box_size, n_publishers, n_subscribers);
   }

   /* closes the pipe */
   close(fman);

   free(message);
   free(register_info);
   return 0;
}
