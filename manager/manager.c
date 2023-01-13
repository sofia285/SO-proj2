#include "logging.h"
#include <string.h>

static void print_usage() {
    fprintf(stderr, "usage: \n"
                    "   manager <register_pipe_name> create <box_name>\n"
                    "   manager <register_pipe_name> remove <box_name>\n"
                    "   manager <register_pipe_name> list\n");
}

int main(int argc, char **argv) {
    if (argc < 4 || argc > 5){ //confirmar que o tamanho do argv é 4 ou 5 aka o correcto
        print_usage();
        exit(1);
    }
    
    //manager <register_pipe_name> <pipe_name> create <box_name>
    //manager <register_pipe_name> <pipe_name> remove <box_name>
    //manager <register_pipe_name> <pipe_name> list

    char const *pipenameManager = argv[1]; //argv[1] é o nome do pipe do manager
    char const *pipenameMBroker = argv[2]; //argv[2] é o nome do pipe do MBroker
    char const *command = argv[3]; //argv[3] é o comando
    if (argc == 5) {
        char const *box_name = argv[4]; //argv[4] é o nome da box
        (void) box_name; //TODO: remover isto
    }
    (void)pipenameManager;//TODO: remover isto
    (void)pipenameMBroker;
    (void)command;

    //encaminhar para cada caso consoante o comando dado
    if (strcmp(command, "create") == 0) {
        //fazer coisas
    } else if (strcmp(command, "remove") == 0) {
        //fazer coisas
    } else if (strcmp(command, "list") == 0) {
        //fazer coisas
    } else {
        print_usage();
        exit(1);
    }
    
    return 0;
}
