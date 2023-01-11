#include "logging.h"



int main(int argc, char **argv) {  //argc= nº de cenas; argv= lista com as coisas; 
    if (argc != 3){ //confirmar que são 3 coisas: mbroker <register_pipe_name> <max_sessions>
        fprintf(stderr, "usage: mbroker <pipename> <max_sessions>\n"); //DEBUG: pode n ser nada disto
        return -1;
    }
    char const *pipename = argv[1]; //argv[1] é o nome do pipe
    (void) pipename; //TODO: remover isto
    int max_sessions = atoi(argv[2]); //argv[2] é o nº máximo de sessões
    if (max_sessions <= 0) {
        fprintf(stderr, "usage: mbroker <pipename> <max_sessions>\n");
        return -1;
    }
    /*if (mbroker_init(pipename, max_sessions) != 0) {
        return -1;
    }
    if (mbroker_run() != 0) {
        return -1;
    }
    if (mbroker_destroy() != 0) {
        return -1;
    }*/
    //fprintf(stderr, "usage: mbroker <pipename>\n");
    WARN("unimplemented"); // TODO: implement

    return -1;
}
