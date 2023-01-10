#include "logging.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/unistd.h>

#define TAMMSG 1000



int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    int fcli, fserv;
    ssize_t n;
    char buf[TAMMSG];

    unlink("/tmp/servidor");
    unlink("/tmp/cliente");

    if (mkfifo("/tmp/servidor", 0777) < 0) {
        exit (1);
    }
    if (mkfifo("/tmp/cliente", 0777) < 0) {
        exit (1);
    }

    if ((fserv = open("/tmp/servidor", O_RDONLY)) < 0) {
	    exit(1);
    }
    if ((fcli = open("/tmp/cliente", O_WRONLY)) < 0) {
        exit(1);
    }
	
    while (1) {
        n = read(fserv, buf, TAMMSG);
        if (n <= 0) break;
        n = write(fcli, buf, TAMMSG);
    }

    close(fserv);
    close(fcli);
    unlink("/tmp/servidor");
    unlink("/tmp/cliente");
    
    return 0;
}
