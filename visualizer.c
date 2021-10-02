#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MEETINGS 0
#define TIME 1
#define PARTICIPANTS 2


struct _ {
    int occurrences;
    char *name;
    int participants;
    int time;
    struct _ *next;
} typedef meetingHost_t;

meetingHost_t *meetingHosts = NULL;
int mode;
int isScaled = 0;
int rows;

int main(int argc, char ** argv) {
    char * fileNames[255];
    int fileNamesIndex = 0;
    FILE * filePointer;

    int argIndex = 1;
    while (argIndex < argc) {
        if (0 == strcmp(argv[argIndex], "-m")) {
            mode = MEETINGS;
        } else if (0 == strcmp(argv[argIndex], "-t")) {
            mode = TIME;
        } else if (0 == strcmp(argv[argIndex], "-p")) {
            mode = PARTICIPANTS;
        } else if (0 == strcmp(argv[argIndex], "-l")) {
            if (argIndex == argc-1) {
                // print length not given error
                return 0;
            } else {
                char * lString = argv[argIndex+1];
                for (int i = 0; lString[i]!='\0' ; i++) {
                    if (isdigit(lString[i]) == 0 && !(i==0 && lString[1]!='\0' && (lString[i] == '+' || lString[i] == '-'))){
                        // print invalid options for length error
                        return 0;
                    }
                }
                int length = atoi(lString);
                if (length<0){
                    // print minus length error
                } else {
                    rows = length;
                }
            }
        } else if (0 == strcmp(argv[argIndex], "--scaled")) {
            isScaled = 1;
        } else {
            // terminate immediately if not .csv format
            int length = strlen(argv[argIndex]);
            char *extension = &(argv[argIndex])[length-4];
            if(0 != strcmp(extension, ".csv")) {
                // print incorrect file message
                return 0;
            }
            fileNames[fileNamesIndex] = argv[argIndex];
            fileNamesIndex++;
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
