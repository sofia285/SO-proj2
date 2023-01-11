#include "logging.h"

int main(int argc, char **argv) { //pub <register_pipe_name> <pipe_name> <box_name>
    if (argc != 4){ 
        fprintf(stderr, "usage: pub <register_pipe_name> <pipe_name> <box_name>\n"); //DEBUG: pode n ser nada disto
        return -1;
    }
    fprintf(stderr, "usage: pub <register_pipe_name> <box_name>\n");
    WARN("unimplemented"); // TODO: implement
    return -1;
}
