
//#include "logging.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdint.h>
#include "../fs/config.h"
#include "../fs/operations.h"
#include "../fs/state.h"
#include "../producer-consumer/producer-consumer.h"


#define REGISTER_INFO_SIZE 2000
#define MESSAGE_SIZE 1025
#define PIPE_NAME_SIZE 256
#define BOX_NAME_SIZE 32
#define PCQ_CAPACITY 100
#define MESSAGE_SIZE 1025
#define CODE_SIZE 1
#define TFS_FILE_STR_SIZE 1024

void *thread_function(void *arg) {

   while(1) {
      //Pop one element from queue
      printf("Starting thread loop \n");
      fflush(stdout);
      void* register_info = pcq_dequeue(arg);
      printf("Pop element from queue \n");
      fflush(stdout);

      //Get code and pipe_name from request
      uint8_t code;
      char pipe_name[PIPE_NAME_SIZE];
      size_t register_info_offset = 0;
      memcpy(&code, register_info, CODE_SIZE); 
      register_info_offset += CODE_SIZE;
      memcpy(pipe_name, register_info + register_info_offset, PIPE_NAME_SIZE);

      if(code != 7){ 
         printf("Getting box name \n");
         fflush(stdout);
         //Get box_name from request
         char box_name[BOX_NAME_SIZE];
         register_info_offset += PIPE_NAME_SIZE;
         memcpy(box_name, register_info + register_info_offset, BOX_NAME_SIZE);

         ssize_t n;
         int fcli;
         void *message = malloc(MESSAGE_SIZE*sizeof(uint8_t)); //needs a free
         if (code == 1) { //publisher
            //Publishes message into TFS (box_name file)
            if ((fcli = open (pipe_name, O_RDONLY)) < 0) {
               printf("Error opening pipe");
               exit(1);
            }
            n = read(fcli, message, MESSAGE_SIZE);
            close(fcli);

            //Get code and text from message
            uint8_t second_code;
            char pub_txt[TFS_FILE_STR_SIZE];
            memcpy(&second_code, message, CODE_SIZE); 
            memcpy(pub_txt, message + CODE_SIZE , TFS_FILE_STR_SIZE);
      
            if(n <= 0) {exit(1);}
            int fhandle = tfs_open(box_name, TFS_O_CREAT);
            if (fhandle < 0) {
               printf("Error opening file");
               exit(1);
            }
            if(tfs_write(fhandle, pub_txt, TFS_FILE_STR_SIZE) < 0){
               printf("Unable to write to file");
               exit(1);
            }
            if(tfs_close(fhandle)<0){
               printf("Unable to close file");
               exit(1);
            }
         }
         else if (code == 2) { //subcriber
            //Reads message from TFS (box_name file)
            int fhandle = tfs_open(box_name, TFS_O_CREAT);
            if (fhandle < 0) {exit(1);}
            char pub_txt[TFS_FILE_STR_SIZE];
            tfs_read(fhandle, pub_txt, TFS_FILE_STR_SIZE);
            tfs_close(fhandle);

            //Sends message to subscriber
            uint8_t second_code = 10;
            memcpy(message, &second_code, CODE_SIZE); 
            memcpy(message + CODE_SIZE , &pub_txt, TFS_FILE_STR_SIZE);
            if ((fcli = open (pipe_name, O_WRONLY)) < 0) {exit(1);}
            n = write(fcli, message, MESSAGE_SIZE);
            if(n <= 0) {exit(1);}
         }
         else if (code == 3) { // manager creates box
            //Creates a file in the TFS
            int fhandle = tfs_open(box_name, TFS_O_CREAT);
            if (fhandle < 0) {exit(1);}
            tfs_close(fhandle);

         }
         else if (code == 5) { // manager removes box
            int fhandle = tfs_open(box_name, TFS_O_TRUNC);
            if (fhandle < 0) {exit(1);}
         }
         free(message);
      }
      free(register_info);
      printf("Done with this client \n");
      fflush(stdout);
   }
}


int main(int argc, char **argv) {

   printf("Running mbroker \n");

   int fserv;
   ssize_t n;

   // verifications
   if (argc != 3) {exit(1);}

   //Create producer-consumer queue
   pc_queue_t pc_queue;
   pcq_create(&pc_queue, PCQ_CAPACITY);

   //Create the threads
   for(int i = 0; i < atoi(argv[2]); i++){
      pthread_t thread_id;
      if (pthread_create(&thread_id, NULL, thread_function, &pc_queue) != 0) {
         perror("pthread_create() error");
         exit(1);
      }
   }
   printf("Created %d threads \n", atoi(argv[2]));
   fflush(stdout);

   //Init tfs with default parameters
   int error = tfs_init(NULL); 
   if (error < 0) {exit(1);}
   printf("Started TFS \n");
   fflush(stdout);
   
   /*printf("Creating fifo %s\n",argv[1] );
   // creates the fifo <register_pipe_name>
   unlink(argv[1]);
   if (mkfifo(argv[1], 0777) < 0) {exit (1);}
   printf("Created fifo \n");
   fflush(stdout);*/

   //Creates a file in the TFS
   int fhandle;
   fhandle = tfs_open("/tmp/bla", TFS_O_CREAT);
   tfs_close(fhandle);
   printf("Created TFS file \n");
   fflush(stdout);

   //Start mbroker loop to receive requests 
   while(1){
      // creates the fifo <register_pipe_name>
      unlink(argv[1]);
      if (mkfifo(argv[1], 0777) < 0) {exit (1);}

      printf("Waiting for a conection \n");
      fflush(stdout);
      // opens the <register_pipe_name> to talk with the client
      if ((fserv = open (argv[1], O_RDONLY)) < 0) {exit(1);}
      
      // recieves the register information from the client
      void *register_info = malloc(REGISTER_INFO_SIZE*sizeof(uint8_t));
      n = read(fserv, register_info, REGISTER_INFO_SIZE);
      if (n <= 0) {exit(1);}

      printf("Adding request to queue \n");
      fflush(stdout);
      // puts the register info into the producer-consumer queue
      pcq_enqueue(&pc_queue, register_info);

      printf("Closing pipe \n");
      fflush(stdout);
      close(fserv);
   }

   return 0;
}
