/*#include "logging.h"

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    fprintf(stderr, "usage: pub <register_pipe_name> <box_name>\n");
    WARN("unimplemented"); // TODO: implement
    return -1;
}*/

//#include "logging.h"

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

  /* Os pipes são criados e destruídos pelo servidor */
  /* Abrir os pipes */
  if (argc < 1) {
    exit(1);
  }

  if ((fserv = open (argv[1], O_WRONLY)) < 0)
	exit(1);
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
}

