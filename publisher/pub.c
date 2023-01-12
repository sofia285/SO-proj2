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

int main(int argc, char **argv) {
    int fcli, fserv;
    char buf[TAMMSG];
    ssize_t n; 
    if (argc != 4){ //FIXME: SOFIA puseste <1 mas acho que é assim
        fprintf(stderr, "usage: pub <register_pipe_name> <pipe_name> <box_name>\n"); //DEBUG: pode n ser nada disto
        exit(1);
    }
    //pub <register_pipe_name> <pipe_name> <box_name>
    char const *pipenamePub = argv[1]; //argv[1] é o nome do pipe do pub
    char const *pipenameMBroker = argv[2]; //argv[2] é o nome do pipe do MBroker
    char const *box_name = argv[3]; //argv[3] é o nome da box
    (void)pipenamePub;//TODO: remover isto tudo se n for preciso 
    (void)pipenameMBroker;
    (void)box_name;
    (void)n;

    if ((fserv = open (argv[1], O_WRONLY)) < 0){
	    exit(1);
    }
    if ((fcli = open ("/tmp/cliente", O_RDONLY)) < 0) {
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
