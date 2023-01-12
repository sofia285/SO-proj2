#include "logging.h"
#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h>

#define TAMMSG 1000

void produzMsg(char *buf) {
    strcpy(buf, "Mensagem de teste");
}

void trataMsg(char *buf) {
    printf("Recebeu: %s\n", buf);
}


int main(int argc, char **argv) { //sub <register_pipe_name> <pipe_name> <box_name>
    int fcli, fserv;
    char buf[TAMMSG];
    ssize_t n;

    if (argc != 3){ 
        fprintf(stderr, "usage: sub <register_pipe_name> <box_name>\n"); //DEBUG: pode n ser nada disto
        exit(1);
    }

    char const *pipenameSub = argv[1]; //argv[1] é o nome do pipe do SubpipenameSub
    char const *pipenameMBroker = argv[2]; //argv[2] é o nome do pipe do MBroker
    char const *box_name = argv[3]; //argv[3] é o nome da box
    (void)pipenameSub;//TODO: remover isto
    (void)pipenameMBroker;
    (void)box_name;

    if ((fserv = open (argv[1], O_WRONLY)) < 0){
	    exit(1);
    }
    if ((fcli = open ("/tmp/cliente", O_RDONLY)) < 0){
        exit(1);
    }

    produzMsg(buf);
    n = write(fserv, buf, TAMMSG);
    n = read(fcli, buf, TAMMSG);
    trataMsg(buf);

    /* Fechar os pipes */
    close(fserv);
    close(fcli);
    return 0;
}
