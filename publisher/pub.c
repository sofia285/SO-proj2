#include "logging.h"

int main(int argc, char **argv) { 
    if (argc != 4){ 
        fprintf(stderr, "usage: pub <register_pipe_name> <pipe_name> <box_name>\n"); //DEBUG: pode n ser nada disto
        return -1;
    }
    //pub <register_pipe_name> <pipe_name> <box_name>
    char const *pipenamePub = argv[1]; //argv[1] é o nome do pipe do pub
    char const *pipenameMBroker = argv[2]; //argv[2] é o nome do pipe do MBroker
    char const *box_name = argv[3]; //argv[3] é o nome da box
    (void)pipenamePub;//TODO: remover isto
    (void)pipenameMBroker;
    (void)box_name;



    fprintf(stderr, "usage: pub <register_pipe_name> <box_name>\n");
    WARN("unimplemented"); // TODO: implement
    return -1;
}
