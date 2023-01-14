/*Começar por criar a queue
Começar por criar o FIFO do servidor (como leitura) <register_pipe_name>
Recebe mensagens de clients pelo fifo: Register info (conter o <pipe_name>)
Encaminhar para uma thread que interage com aquele cliente (pôr na queue) funçao:"pcq_enqueue"
o thread:
        -Tira um cliente da queue com: "pcq_dequeue"
        -Cria o novo pipe <pipe_name> que pode ser leitura ou escrita dependendo do tipo de cliente

*/
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

void *thread_function(void *arg) {
   (void) arg; 
   //uint8_t code = *(uint8_t*)arg;
   //printf("Thread created. Client with code:%d, named_pipe_path:%s, box_name:%s", code, (char*)(&arg[1]), (char*)(&arg[33]));
   //fflush(stdout);
   return arg;
}


int main(int argc, char **argv) {

   printf("Running mbroker \n");
   fflush(stdout);

   int fserv, fpub;
   ssize_t n;
   char register_info[REGISTER_INFO_SIZE], pipe_name[PIPE_NAME_SIZE], 
   box_name[BOX_NAME_SIZE], message[MESSAGE_SIZE];

   // verifications
   if (argc != 4 || strcmp(argv[1], "mbroker")) {
      exit(1);
   }

   printf("TFS init \n");  
   fflush(stdout); 
   int error = tfs_init(NULL); //init tfs with default parameters
   if (error < 0) {exit(1);}
   /*
   printf("Initialize returned %d. TFS write \n", a);
   int fhandle;
   fhandle = tfs_open("/tmp/bla", TFS_O_CREAT);
   ssize_t bytes = tfs_write(fhandle, "Bom dia", 10);
   tfs_close(fhandle);

   printf("Fhandle: %d. Wrote %ld bytes. TFS read \n", fhandle, bytes);
   char msg[10] ;
   fhandle = tfs_open("/tmp/bla", TFS_O_CREAT);
   ssize_t read_bytes = tfs_read(fhandle, msg, 10);
   printf("Read %ld bytes. Char: %c \n", read_bytes, msg[2]);
   printf("Mengagem: %s \n", msg);
   tfs_close(fhandle);

   tfs_unlink("/tmp/bla"); //caso o manager feche
   */

   pc_queue_t pc_queue;//init producer-consumer queue
   pcq_create(&pc_queue, PCQ_CAPACITY);

   unlink(argv[1]);

   // creates the fifo <register_pipe_name>
   if (mkfifo(argv[1], 0777) < 0) {
      exit (1);
   }

   // opens the <register_pipe_name> to talk with the client
   if ((fserv = open (argv[1], O_RDONLY)) < 0) {
	   exit(1);
   }

   // recieves the register information from the client
   n = read(fserv, register_info, REGISTER_INFO_SIZE);
   if (n <= 0) {exit(1);}

   //Create a thread
   pthread_t thread_id;
   if (pthread_create(&thread_id, NULL, thread_function, register_info) != 0) {
    perror("pthread_create() error");
    exit(1);
   }
   
   uint8_t code = register_info[0];

   if (code == 1) {
      for(int i = 1; i <= 257; i++) {pipe_name[i - 1] = register_info[i];}
      for(int i = 258; i <= 290; i++) {box_name[i - 1] = register_info[i];}
      if ((fpub = open (pipe_name, O_RDONLY)) < 0) {exit(1);}
      n = read(fpub, message, MESSAGE_SIZE);
      int fhandle = tfs_open(box_name, TFS_O_CREAT);
      tfs_write(fhandle, message, MESSAGE_SIZE);
      tfs_close(fhandle);
   }
   else if (code == 2) {
      for(int i = 1; i <= 257; i++) {pipe_name[i - 1] = register_info[i];}
      for(int i = 258; i <= 290; i++) {box_name[i - 1] = register_info[i];}
      if ((fpub = open (pipe_name, O_RDONLY)) < 0) {exit(1);}
      n = read(fpub, message, MESSAGE_SIZE);
      int fhandle = tfs_open(box_name, TFS_O_CREAT);
      tfs_read(fhandle, message, MESSAGE_SIZE);
      tfs_close(fhandle);
      
   }
   sleep(1);
   return 0;
}
