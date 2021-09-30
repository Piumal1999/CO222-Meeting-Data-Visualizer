#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct _ {
    int occurrences;
    char *name;
    int participants;
    int time;
    struct _ *next;
} typedef meetingHost_t;

meetingHost_t *meetingHosts = NULL;

int main(int argc, char ** argv) {
    char * fileNames[255];
    int fileNamesIndex = 0;
    FILE * filePointer;

    int argIndex = 1;
    while (argIndex < argc) {
        if (0 == strcmp(argv[argIndex], "-m")) {
            // change mode
        } else if (0 == strcmp(argv[argIndex], "-t")) {
            // change mode
        } else if (0 == strcmp(argv[argIndex], "-p")) {
            // change mode
        } else if (0 == strcmp(argv[argIndex], "-l")) {
            // set graph height
        } else if (0 == strcmp(argv[argIndex], "--scaled")) {
            // set scaled mode on
        } else {
            // terminate immediately if not .csv format
            filenames[filenameIndex] = argv[argIndex];
            filenameIndex++;
        }
        argIndex++;
    }

    if (fileNamesIndex == 0) {
        // print no files error
        return 0;
    }

    for (int i = 0; i <= fileNamesIndex; i++) {
        filePointer = NULL;
        filePointer = fopen(*fileNames + i, "r");
        if (filePointer == NULL) {
            // terminate immediately
        } else {
            // read file
        }
    }

    // plot the graph according to the mode
}
