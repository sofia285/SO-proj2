#include "logging.h"
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


#define REGISTER_INFO_SIZE 289
#define MESSAGE_SIZE 1025
#define MANAGER_MESSAGE_SIZE 1029
#define PIPE_NAME_SIZE 256
#define BOX_NAME_SIZE 32
#define PCQ_CAPACITY 100
#define MESSAGE_SIZE 1025
#define CODE_SIZE 1
#define TFS_FILE_STR_SIZE 1024
#define LIST_MESSAGE_SIZE 58


typedef struct box{
   char box_name[BOX_NAME_SIZE];
   uint64_t box_size; //box size (bytes)
   uint64_t n_publishers;
   uint64_t n_subscribers;
   struct box* next;
}Box;


Box *box_list;

void *thread_function(void *arg) {

   while(1) {
      //Pop one element from queue
      void* register_info = pcq_dequeue(arg);

      //Get code and pipe_name from request
      uint8_t code;
      char pipe_name[PIPE_NAME_SIZE];
      size_t register_info_offset = 0;
      memcpy(&code, register_info, CODE_SIZE); 
      register_info_offset += CODE_SIZE;
      memcpy(pipe_name, register_info + register_info_offset, PIPE_NAME_SIZE);

      ssize_t n;
      int fcli;
      if(code != 7){ 
         //Get box_name from request
         char box_name[BOX_NAME_SIZE];
         register_info_offset += PIPE_NAME_SIZE;
         memcpy(box_name, register_info + register_info_offset, BOX_NAME_SIZE);
         void *message = malloc(MESSAGE_SIZE*sizeof(uint8_t)); //needs a free
         if (code == 1) { //publisher
            //Open pipe
            if ((fcli = open (pipe_name, O_RDONLY)) < 0) {
            printf("Error opening pipe \n");
            exit(1);
            }

            //Open TFS file
            int fhandle = tfs_open(box_name, TFS_O_APPEND);
            if (fhandle < 0) {
               printf("Error opening file \n");
               exit(1);
            }

            while(1){
               //Read message from publisher
               n = read(fcli, message, MESSAGE_SIZE);
               if(n == 0){
                  close(fcli);
                  if(tfs_close(fhandle)<0){
                     printf("Unable to close file");
                     exit(1);
                  }
                  break;
               }

               //Get code and text from message
               uint8_t second_code;
               char pub_txt[TFS_FILE_STR_SIZE];
               memcpy(&second_code, message, CODE_SIZE); 
               memcpy(pub_txt, message + CODE_SIZE , TFS_FILE_STR_SIZE);

               if(second_code != 9){
                  fprintf(stdout, "Error in the comunicaton protocol, expected code 9 from publisher\n");
                  exit(1);
               }
      
               //Publishes message into TFS (box_name file)
               if(tfs_write(fhandle, pub_txt, TFS_FILE_STR_SIZE) < 0){
                  printf("Unable to write to TFS file");
                  exit(1);
               }
            }
         }
         else if (code == 2) { //subcriber
            //Open TFS file
            int fhandle = tfs_open(box_name, TFS_O_CREAT);
            if (fhandle < 0) {exit(1);}

            //Open pipe
            if ((fcli = open (pipe_name, O_WRONLY)) < 0) {exit(1);}

            while(1){
               //Reads message from TFS (box_name file)
               char pub_txt[TFS_FILE_STR_SIZE];
               n = tfs_read(fhandle, pub_txt, TFS_FILE_STR_SIZE);
               if(n == 0){
                  tfs_close(fhandle);
                  close(fcli);
                  break;
               }
               //Sends message to subscriber
               uint8_t second_code = 10;
               memcpy(message, &second_code, CODE_SIZE); 
               memcpy(message + CODE_SIZE , &pub_txt, TFS_FILE_STR_SIZE);
               n = write(fcli, message, MESSAGE_SIZE);
               if(n <= 0) {exit(1);}
            }
         }
         else if (code == 3) { // manager creates box
            int32_t return_code = 0;
            char error_msg[TFS_FILE_STR_SIZE] = {'\0'};
            //Creates a file in the TFS
            int fhandle = tfs_open(box_name, TFS_O_CREAT);
            if (fhandle < 0) {
               return_code = -1;
               strcpy(error_msg, "Unable to create box");
            }
            tfs_close(fhandle);

            //Add to list of boxes
            box_list->next = (malloc(sizeof(Box)));
            strcpy(box_list->next->box_name,box_name);
            box_list->next->box_size = 0;
            box_list->next->n_publishers = 0;
            box_list->next->n_subscribers = 0;
            box_list->next->next = NULL;

            //Sends message to manager
            void *manager_message = malloc(MANAGER_MESSAGE_SIZE*sizeof(uint8_t));
            uint8_t second_code = 4;
            memcpy(manager_message, &second_code, CODE_SIZE); 
            memcpy(manager_message + CODE_SIZE, &return_code, sizeof(int32_t)); 
            memcpy(manager_message + CODE_SIZE + sizeof(int32_t), error_msg, TFS_FILE_STR_SIZE);
            if ((fcli = open (pipe_name, O_WRONLY)) < 0) {exit(1);}
            n = write(fcli, manager_message, MANAGER_MESSAGE_SIZE);
            if(n <= 0) {exit(1);}
         }
         else if (code == 5) { // manager removes box
            int32_t return_code = 0;
            char error_msg[TFS_FILE_STR_SIZE] = {'\0'};
            //Creates a file in the TFS
            int fhandle = tfs_unlink(box_name);
            if (fhandle < 0) {
               return_code = -1;
               strcpy(error_msg, "Unable to remove box");
            }
            tfs_close(fhandle);

            //Sends message to manager
            void *manager_message = malloc(MANAGER_MESSAGE_SIZE*sizeof(uint8_t)); //needs a free
            uint8_t second_code = 6;
            memcpy(manager_message, &second_code, CODE_SIZE); 
            memcpy(manager_message + CODE_SIZE, &return_code, sizeof(int32_t)); 
            memcpy(manager_message + CODE_SIZE + sizeof(int32_t), error_msg, TFS_FILE_STR_SIZE);
            if ((fcli = open (pipe_name, O_WRONLY)) < 0) {exit(1);}
            n = write(fcli, manager_message, MANAGER_MESSAGE_SIZE);
            if(n <= 0) {exit(1);}
         }
         free(message);
      }
      else if (code == 7){ //Send the list to the manager
            //Sends message to manager
            ssize_t offset = 0;
            void *manager_message = malloc(LIST_MESSAGE_SIZE*sizeof(uint8_t));
            uint8_t second_code = 8;
            uint8_t last = 0; //1 if it is the last element (otherwise 0) 

            if(box_list->next->box_name!= NULL){
               memcpy(manager_message, &second_code, CODE_SIZE); 
               offset+=CODE_SIZE;
               memcpy(manager_message + offset, &last, CODE_SIZE); 
               offset+=CODE_SIZE;
               memcpy(manager_message + offset, box_list->next->box_name, BOX_NAME_SIZE);
               offset+=BOX_NAME_SIZE;
               memcpy(manager_message + offset, &(box_list->next->box_size), sizeof(uint64_t));
               offset+=8;
               memcpy(manager_message + offset, &(box_list->next->n_publishers), sizeof(uint64_t));
               offset+=8;         
               memcpy(manager_message + offset, &(box_list->next->n_subscribers), sizeof(uint64_t));

               if ((fcli = open (pipe_name, O_WRONLY)) < 0) {exit(1);}
               n = write(fcli, manager_message, LIST_MESSAGE_SIZE);
               if(n <= 0) {exit(1);}

               free(manager_message);
            }
      }
      free(register_info);
      
   }
}


int main(int argc, char **argv) {
   box_list = malloc(sizeof(Box));
   box_list->next = NULL;
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

   //Init tfs with default parameters
   int error = tfs_init(NULL); 
   if (error < 0) {exit(1);}

   //Start mbroker loop to receive requests 
   while(1){
      // creates the fifo <register_pipe_name>
      unlink(argv[1]);
      if (mkfifo(argv[1], 0777) < 0) {exit (1);}

      // opens the <register_pipe_name> to talk with the client
      if ((fserv = open (argv[1], O_RDONLY)) < 0) {exit(1);}
      
      // recieves the register information from the client
      void *register_info = malloc(REGISTER_INFO_SIZE*sizeof(uint8_t));
      n = read(fserv, register_info, REGISTER_INFO_SIZE);
      if (n <= 0) {exit(1);}

      // puts the register info into the producer-consumer queue
      pcq_enqueue(&pc_queue, register_info);

      close(fserv);
   }

   return 0;
}
