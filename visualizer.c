#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define FALSE 0
#define TRUE 1

// define modes
#define DEFAULT 0
#define MEETINGS 1
#define PARTICIPANTS 2
#define TIME 3

// structure to keep the data of a meeting host
struct _ {
    char *name; // name
    int value;  // value according to the mode
    struct _ *next;
} typedef meetingHost_t;

void printInstructionMessage();
void setRowCount(char *);
int getTotalOfValues(meetingHost_t *);
meetingHost_t *getSortedList();
int getMaxLengthOfNames(meetingHost_t *);
int getMaxValueOfSortedList(meetingHost_t *);
int getNumberOfDigits(int);

char *programName; // variable to keep the name of the program
meetingHost_t *meetingHosts = NULL; // linked list for meeting hosts
int rows = 10;  // variable to keep the required number of rows (default value = 10)

int main(int argc, char **argv) {
    int mode = DEFAULT; // variable to keep the running mode
    int isScaled = FALSE;   // variable to keep the scaled mode on/off
    programName = argv[0];  // set the program name

    char *fileNames[255];  // pointer for file names
    int fileNamesIndex = 0; // variable to keep the file name index

    int argIndex = 1;   // variable to index arguments

    // check the arguments
    while (argIndex < argc) {
        if (argv[argIndex][0] == '-') {
            if (0 == strcmp(argv[argIndex], "-m")) {
                // check and set meeting mode
                if (mode == DEFAULT || mode == MEETINGS) {
                    mode = MEETINGS;
                } else {
                    printf("Cannot plot multiple parameters in same graph.\n");
                    printInstructionMessage();
                    return 0;
                }
            } else if (0 == strcmp(argv[argIndex], "-t")) {
                // check and set time mode
                if (mode == DEFAULT || mode == TIME) {
                    mode = TIME;
                } else {
                    printf("Cannot plot multiple parameters in same graph.\n");
                    printInstructionMessage();
                    return 0;
                }
            } else if (0 == strcmp(argv[argIndex], "-p")) {
                // check and set participant mode
                if (mode == DEFAULT || mode == PARTICIPANTS) {
                    mode = PARTICIPANTS;
                } else {
                    printf("Cannot plot multiple parameters in same graph.\n");
                    printInstructionMessage();
                    return 0;
                }
            } else if (0 == strcmp(argv[argIndex], "--scaled")) {
                // set scaling on
                isScaled = TRUE;
            } else if (0 == strcmp(argv[argIndex], "-l")) {
                // check and set the rows count
                if (argIndex == argc - 1) {
                    // terminate if no options for -l
                    printf("Not enough options for [-l]\n");
                    printInstructionMessage();
                    return 0;
                } else {
                    // check and set the row count
                    argIndex++;
                    setRowCount(argv[argIndex]);
                }
            } else {
                // terminate if invalid arguments
                printf("Invalid option [%s]\n", argv[argIndex]);
                printInstructionMessage();
                return 0;
            }
        } else {
            // terminate immediately if file is not in .csv format
            int lengthOfFileName = strlen(argv[argIndex]);
            char *extension = &(argv[argIndex])[lengthOfFileName - 4];
            if (0 != strcmp(extension, ".csv")) {
                printf("Only .csv files should be given as inputs.\n");
                return 0;
            }
            fileNames[fileNamesIndex] = argv[argIndex]; // add the file name to file names list
            fileNamesIndex++;
        }
        argIndex++; // to set the index to next argument
    }

    if (fileNamesIndex == 0) {
        // terminate if no files are given
        printf("No input files were given\n");
        printInstructionMessage();
        return 0;
    }

    // read files
    for (int i = 0; i < fileNamesIndex; i++) {
        FILE *filePointer = fopen(*(fileNames + i), "r");  // open the file
        if (filePointer == NULL) {
            // terminate if the file is not opened
            printf("Cannot open one or more given files\n");
            return 0;
        } else {
            // variables to keep the values which are read next
            char name[256];
            char participants[128];
            char time[128];
            char line[512];
            // read the file line by line
            while (fgets(line, 512, filePointer) != NULL) {
                // extract the variables from the line
                int lineScanRes = sscanf(line, "%[^,],%[^,],%[^\n]", name, participants, time);
                if (lineScanRes != 3) {
                    // terminate if line is wrong
                    printf("File/s contain wrong entries.\n");
                    return 0;
                } else {
                    // check whether the meeting host with the name already exists
                    meetingHost_t *current;
                    for (current = meetingHosts; current != NULL; current = current->next) {
                        if (strcmp(current->name, name) == 0) {
                            break;
                        }
                    }
                    if (current == NULL) {
                        // create a new meeting host if no host exists with the same name
                        meetingHost_t *newHost = (meetingHost_t *) malloc(sizeof(meetingHost_t));
                        newHost->name = (char *) malloc(sizeof(char) * (strlen(name)) + 1);
                        strcpy(newHost->name, name);

                        // add the data according to the selected mode
                        switch (mode) {
                            case MEETINGS:
                                newHost->value = 1;
                                break;
                            case PARTICIPANTS:
                                newHost->value = atoi(participants);
                                break;
                            case TIME: {
                                // split time string and extract hours and minutes
                                int hours, minutes = 0;
                                char *token = strtok(time, ":");
                                hours = atoi(token);
                                if (token != NULL) {
                                    token = strtok(NULL, ":");
                                    minutes = atoi(token);
                                }
                                newHost->value = hours * 60 + minutes;
                                break;
                            }
                            default:
                                newHost->value = 1;
                        }
                        // attach the new meeting host as the head of the linked list
                        newHost->next = meetingHosts;
                        meetingHosts = newHost;
                    } else {
                        // add the data according to the mode, if the meeting host already exists
                        switch (mode) {
                            case MEETINGS:
                                current->value++;    // add 1 to meeting count
                                break;
                            case PARTICIPANTS:
                                current->value += atoi(participants);    // add participants
                                break;
                            case TIME: {
                                // split time string and extract hours and minutes
                                int hours, minutes = 0;
                                char *token = strtok(time, ":");
                                hours = atoi(token);
                                if (token != NULL) {
                                    token = strtok(NULL, ":");
                                    minutes = atoi(token);
                                }
                                current->value += hours * 60 + minutes;  // add the time
                                break;
                            }
                            default:
                                current->value++;
                        }
                    }
                }
            }
        }
    }

    if (meetingHosts == NULL) {
        // terminate if the linked list is null
        printf("No data to process\n");
        return 0;
    }

    int total = getTotalOfValues(meetingHosts); // get sum of entries in the selected mode
    meetingHost_t *sortedList = getSortedList();  // sort the linked list by selected mode

    // calculate graph dimensions
    int spaceForName = getMaxLengthOfNames(sortedList);
    int spaceForValue = getNumberOfDigits(getMaxValueOfSortedList(sortedList));
    int remainingSpace = 80 - spaceForName - spaceForValue - 3;

    float blockValue;   // value represented by single block in the graph
    // set the block value according to status of scaling option
    if (isScaled) {
        // block value depends on the maximum value
        blockValue = 1.0 * getMaxValueOfSortedList(sortedList) / remainingSpace;
    } else {
        blockValue = 1.0 * total / remainingSpace;  // block value depends on the total
    }

    printf("\n");   // top margin

    // print the graph by iterating through the sorted list
    for (meetingHost_t *current = sortedList; current != NULL; current = current->next) {
        int blocks = current->value / blockValue;    // calculate the required number of blocks
        //  print first line of the row
        printf(" %*s \u2502", -1 * spaceForName, "");   // set the padding
        for (int i = 0; i < blocks; i++) {  // print blocks
            printf("\u2591");
        }
        printf("\n");

        //  print second line of the row
        printf(" %*s \u2502", -1 * spaceForName, current->name);
        for (int i = 0; i < blocks; i++) {  // print blocks
            printf("\u2591");
        }
        printf("%d", current->value);    // print the number of meetings
        printf("\n");

        //  print 3rd line of the row
        printf(" %*s \u2502", -1 * spaceForName, "");
        for (int i = 0; i < blocks; i++) {  // print blocks
            printf("\u2591");
        }
        printf("\n");

        printf(" %*s \u2502\n", -1 * spaceForName, ""); // print the spacing line
    }

    // print the x-axis (last line)
    printf(" %*s \u2514", -1 * spaceForName, "");
    for (int i = 0; i < remainingSpace + spaceForValue; i++) {
        printf("\u2500");
    }
    printf("\n");

    return 0;   // terminate the program
}

// function to print the instruction message with program name
void printInstructionMessage() {
    printf("usage: %s [-l length] [-m | -t | -p] [--scaled] filename1 filename2 ..\n", programName);
}

// function check the -l argument input and set the required row count
void setRowCount(char *rowString) {
    for (int i = 0; rowString[i] != '\0'; i++) {
        if (isdigit(rowString[i]) == FALSE &&
            !(i == 0 && rowString[1] != '\0' && (rowString[i] == '+' || rowString[i] == '-'))) {
            // terminate if the value is not a number
            printf("Invalid options for [-l]\n");
            printInstructionMessage();
            exit(0);
        }
    }
    int length = atoi(rowString);
    if (length < 0) {
        // terminate if the length is negative
        printf("Invalid option(negative) for [-l]\n");
        printInstructionMessage();
        exit(0);
    } else if (length == 0) {
        // terminate with no outputs if row count is 0
        exit(0);
    } else {
        // set the required row count
        rows = length;
    }
}

// function to get the total of selected mode in a linked list
int getTotalOfValues(meetingHost_t *sortedList) {
    int total = 0;
    for (meetingHost_t *current = sortedList; current != NULL; current = current->next) {
        total += current->value;
    }
    return total;
}

// function to sort the meetingHost_t linked list in the selected mode
meetingHost_t *getSortedList() {
    meetingHost_t *sortedList = NULL;   // new pointer for sorted list
    meetingHost_t *sortedListNode = NULL;   // pointer for sorted list tail node

    // loop through the meeting hosts linked list until end or until the requested number of rows are found
    for (int i = 0; meetingHosts != NULL && i < rows; i++) {
        // find the host with the highest value
        meetingHost_t *tempHighest = meetingHosts, *previousToHighest = NULL, *previous = NULL;
        for (meetingHost_t *current = meetingHosts; current != NULL; previous = current, current = current->next) {
            if (tempHighest->value <= current->value) {
                tempHighest = current;
                previousToHighest = previous;
            }
        }

        // attach the highest one to the sorted linked list
        if (sortedList == NULL) {
            sortedList = tempHighest;
            sortedListNode = sortedList;
        } else {
            sortedListNode->next = tempHighest;
            sortedListNode = sortedListNode->next;
        }

        // remove the highest one from the unsorted linked list
        if (previousToHighest == NULL) {
            meetingHosts = tempHighest->next;
        } else {
            previousToHighest->next = tempHighest->next;
        }

    }
    sortedListNode->next = NULL;
    return sortedList;  // return the head of sorted list
}

// function to get the maximum name length in a linked list
int getMaxLengthOfNames(meetingHost_t *sortedList) {
    int maxLength = 0;
    for (meetingHost_t *current = sortedList; current != NULL; current = current->next) {
        if (strlen(current->name) > maxLength) {
            maxLength = strlen(current->name);
        }
    }
    return maxLength;
}

// function to get the maximum value of a sorted linked list in the selected mode
// always the first element contains the maximum value
int getMaxValueOfSortedList(meetingHost_t *sortedList) {
    return sortedList->value;
}

// function to get the number of digits in a number
int getNumberOfDigits(int number) {
    int digits = 0;
    while (number != 0) {
        number = number / 10;
        digits++;
    }
    return digits;
}
