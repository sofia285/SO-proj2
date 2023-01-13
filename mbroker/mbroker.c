#include "logging.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/unistd.h>
#include <stdlib.h>
#include <string.h>

#define TAMMSG 1000

void server(char* register_name_pipe, char* max_sessions);


int main(int argc, char **argv) {  //argc= nº de cenas; argv= lista com as coisas; 

    if (argc != 3){ //confirmar que são 3 coisas: mbroker <register_pipe_name> <max_sessions>
        printf("Entrou no argc != 3\n");
        printf("argc: %d", argc);

        fprintf(stderr, "usage: mbroker <pipename> <max_sessions>\n"); //DEBUG: pode n ser nada disto
        exit(1);
    }

    char *pipename = argv[1]; //argv[1] é o nome do pipe
    char *max_sessions = argv[2]; //argv[2] é o nº máximo de sessões

    if (atoi(max_sessions) < 1) {
        printf("Entrou no atoi\n");
        printf("max_sessions: %s", max_sessions);
        fprintf(stderr, "usage: mbroker <pipename> <max_sessions>\n");
        exit(1);
    }

    server(pipename, max_sessions);
    printf("\t----DEBUG----\n");
    return 0;
}

void server(char* register_name_pipe, char* max_sessions) {

    int fcli, fserv;
    ssize_t n;
    char buf[TAMMSG];

    /*if (register_name_pipe ja existe){
        exit(1);
    }
    if (n_sessions >= max_sessions){
        exit(1);
    }*/

    printf("register_name_pipe: %s | max_sessions: %s\n", register_name_pipe, max_sessions);

    unlink(register_name_pipe);
    unlink("/tmp/cliente");

    if (mkfifo(register_name_pipe, 0777) < 0) {
        exit (1);
    }
    if (mkfifo("/tmp/cliente", 0777) < 0) {
        exit (1);
    }

    
    if ((fserv = open(register_name_pipe, O_RDONLY)) < 0) {
        exit(1);
    }
    if ((fcli = open("/tmp/cliente", O_WRONLY)) < 0) {
        exit(1);
    }
    
    printf("Awaiting message\n");
    fflush(stdout);

    while (1) {
        n = read(fserv, buf, TAMMSG);
        if (n <= 0) break;
        n = write(fcli, buf, TAMMSG);
    }

    close(fserv);
    close(fcli);
    unlink(register_name_pipe);
    unlink("/tmp/cliente");
}
