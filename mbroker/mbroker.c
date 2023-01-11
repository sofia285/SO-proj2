//#include "logging.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/unistd.h>
#include <stdlib.h>
#include <string.h>

#define TAMMSG 1000

void server(char* register_name_pipe, char* max_sessions);


int main(int argc, char **argv) {
    
    printf("%s\n", argv[0]);
            
    if (argc != 3) {
        exit(1);
    }
    server(argv[1], argv[2]);
    
    return 0;
}

void server(char* register_name_pipe, char* max_sessions) {

    int fcli, fserv;
    ssize_t n;
    char buf[TAMMSG];

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
