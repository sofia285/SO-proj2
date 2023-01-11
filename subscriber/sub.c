#include "logging.h"

int main(int argc, char **argv) { //sub <register_pipe_name> <pipe_name> <box_name>
    if (argc != 3){ 
        fprintf(stderr, "usage: sub <register_pipe_name> <box_name>\n"); //DEBUG: pode n ser nada disto
        return -1;
    }
    
    char const *pipenameSub = argv[1]; //argv[1] é o nome do pipe do SubpipenameSub
    char const *pipenameMBroker = argv[2]; //argv[2] é o nome do pipe do MBroker
    char const *box_name = argv[3]; //argv[3] é o nome da box
    (void)pipenameSub;//TODO: remover isto
    (void)pipenameMBroker;
    (void)box_name;

    fprintf(stderr, "usage: sub <register_pipe_name> <box_name>\n");
    WARN("unimplemented"); // TODO: implement
    return -1;
}
