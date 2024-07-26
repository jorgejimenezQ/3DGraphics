#include "errors.h"

void error(char* message) {
    fprintf(stderr, "%s\n", message);
    exit(1);
}

void notImplemented(int noError) {
    if (noError) {
        fprintf(stderr, "Error: Function not implemented.\n");
        return;
    }
    // Print an error message and exit the program
    fprintf(stderr, "Error: Function not implemented.\n");
    exit(1);
}

