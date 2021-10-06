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

struct _ {
    int meetings;
    char *name;
    int participants;
    int time;
    struct _ *next;
} typedef meetingHost_t;

void printInstructionMessage();
int getMaxLengthOfNames(meetingHost_t *);
int getMaxValueOfSortedList(meetingHost_t *, int);
int getNumberOfDigits(int);
meetingHost_t *getSortedListByMode(int);
meetingHost_t *sortListByMeetings();
meetingHost_t *sortListByParticipants();
meetingHost_t *sortListByTime();
int getTotalByMode(meetingHost_t *, int);
int getTotalMeetings(meetingHost_t *);
int getTotalParticipants(meetingHost_t *);
int getTotalTime(meetingHost_t *);
void setRowCount(char *);

char * programName;
meetingHost_t *meetingHosts = NULL;
int rows = 10;

int main(int argc, char ** argv) {
    int mode = DEFAULT;
    int isScaled = FALSE;
    programName = argv[0];

    char * fileNames[255];
    int fileNamesIndex = 0;
    FILE * filePointer;

    int argIndex = 1;
    while (argIndex < argc) {
        if (argv[argIndex][0] == '-') {
            if (0 == strcmp(argv[argIndex], "-m")) {
                if (mode == DEFAULT || mode == MEETINGS) {
                    mode = MEETINGS;
                } else {
                    printf("Cannot plot multiple parameters in same graph.\n");
                    printInstructionMessage();
                    return 0;
                }
            } else if (0 == strcmp(argv[argIndex], "-t")) {
                if (mode == DEFAULT || mode == TIME) {
                    mode = TIME;
                } else {
                    printf("Cannot plot multiple parameters in same graph.\n");
                    printInstructionMessage();
                    return 0;
                }
            } else if (0 == strcmp(argv[argIndex], "-p")) {
                if (mode == DEFAULT || mode == PARTICIPANTS) {
                    mode = PARTICIPANTS;
                } else {
                    printf("Cannot plot multiple parameters in same graph.\n");
                    printInstructionMessage();
                    return 0;
                }
            } else if (0 == strcmp(argv[argIndex], "--scaled")) {
                isScaled = TRUE;
            } else if (0 == strcmp(argv[argIndex], "-l")) {
                if (argIndex == argc - 1) {
                    printf("Not enough options for [-l]\n");
                    printInstructionMessage();
                    return 0;
                } else {
                    argIndex++;
                    setRowCount(argv[argIndex]);
                }
            } else {
                printf("Invalid option [%s]\n", argv[argIndex]);
                printInstructionMessage();
                return 0;
            }
        } else {
            // terminate immediately if not .csv format
            int lengthOfFileName = strlen(argv[argIndex]);
            char *extension = &(argv[argIndex])[lengthOfFileName - 4];
            if (0 != strcmp(extension, ".csv")) {
                printf("Only .csv files should be given as inputs.\n");
                return 0;
            }
            fileNames[fileNamesIndex] = argv[argIndex];
            fileNamesIndex++;
        }
        argIndex++;
    }

    if (fileNamesIndex == 0) {
        printf("No input files were given\n");
        printInstructionMessage();
        return 0;
    }

    for (int i = 0; i < fileNamesIndex; i++) {
        filePointer = NULL;
        filePointer = fopen(*(fileNames + i), "r");
        if (filePointer == NULL) {
            printf("Cannot open one or more given files\n");
            return 0;
        } else {
            char name[256];
            char participants[256];
            char time[256];
            char line[780];
            while (fgets (line, 780, filePointer) != NULL) {
                int lineScanRes = sscanf(line, "%[^,],%[^,],%[^\n]", name, participants, time);
                if (lineScanRes != 3) {
                    printf("File/s contain wrong entries.\n");
                    return 0;
                } else {
                    meetingHost_t *current;
                    for (current = meetingHosts; current != NULL; current = current->next) {
                        if (strcmp(current->name, name) == 0) {
                            break;
                        }
                    }
                    if (current == NULL) {
                        meetingHost_t *newHost = (meetingHost_t *) malloc(sizeof(meetingHost_t));
                        newHost->name = (char *) malloc(sizeof(char) * (strlen(name)) + 1);
                        strcpy(newHost->name, name);

                        switch (mode) {
                            case MEETINGS:
                                newHost->meetings = 1;
                                break;
                            case PARTICIPANTS:
                                newHost->participants = atoi(participants);
                                break;
                            case TIME: {
                                int hours, minutes = 0;
                                char *token = strtok(time, ":");
                                hours = atoi(token);
                                if (token != NULL) {
                                    token = strtok(NULL, ":");
                                    minutes = atoi(token);
                                }
                                newHost->time = hours * 60 + minutes;
                                break;
                            }
                            default:
                                newHost->meetings = 1;
                        }
                        newHost->next = meetingHosts;
                        meetingHosts = newHost;
                    } else {
                        switch (mode) {
                            case MEETINGS:
                                current->meetings++;
                                break;
                            case PARTICIPANTS:
                                current->participants += atoi(participants);
                                break;
                            case TIME: {
                                int hours, minutes = 0;
                                char *token = strtok(time, ":");
                                hours = atoi(token);
                                if (token != NULL) {
                                    token = strtok(NULL, ":");
                                    minutes = atoi(token);
                                }
                                current->time += hours * 60 + minutes;
                                break;
                            }
                            default:
                                current->meetings++;
                        }
                    }
                }
            }
        }
    }

    if (meetingHosts == NULL) {
        printf("No data to process\n");
        return 0;
    }

    int total = getTotalByMode(meetingHosts, mode); // get sum of entries in the selected mode
    meetingHost_t *sortedList = getSortedListByMode(mode);  // sort the linked list by selected mode

    // calculate graph dimensions
    int spaceForName = getMaxLengthOfNames(sortedList);
    int spaceForValue = getNumberOfDigits(getMaxValueOfSortedList(sortedList, mode));
    int remainingSpace = 80 - spaceForName - spaceForValue - 3;

    float blockValue;   // value represented by single block in the graph
    if (isScaled) {
        blockValue = 1.0 * getMaxValueOfSortedList(sortedList, mode) / remainingSpace;
    } else {
        blockValue = 1.0 * total / remainingSpace;
    }

    printf("\n");   // top margin

    // print graph data
    for (meetingHost_t *current = sortedList; current != NULL; current = current->next) {
        if (mode == MEETINGS) {
            int blocks = current->meetings / blockValue;
            printf(" %*s \u2502", -1 * spaceForName, ""); // line 1
            for (int i = 0; i < blocks; i++) {
                printf("\u2591");
            }
            printf("\n");
            printf(" %*s \u2502", -1 * spaceForName, current->name);    // line 2
            for (int i = 0; i < blocks; i++) {
                printf("\u2591");
            }
            printf("%d", current->meetings);
            printf("\n");
            printf(" %*s \u2502", -1 * spaceForName, ""); // line 3
            for (int i = 0; i < blocks; i++) {
                printf("\u2591");
            }
            printf("\n");
        } else if (mode == PARTICIPANTS) {
            int blocks = current->participants / blockValue;
            printf(" %*s \u2502", -1 * spaceForName, ""); // line 1
            for (int i = 0; i < blocks; i++) {
                printf("\u2591");
            }
            printf("\n");
            printf(" %*s \u2502", -1 * spaceForName, current->name);    // line 2
            for (int i = 0; i < blocks; i++) {
                printf("\u2591");
            }
            printf("%d", current->participants);
            printf("\n");
            printf(" %*s \u2502", -1 * spaceForName, ""); // line 3
            for (int i = 0; i < blocks; i++) {
                printf("\u2591");
            }
            printf("\n");
        } else if (mode == TIME) {
            int blocks = current->time / blockValue;
            printf(" %*s \u2502", -1 * spaceForName, ""); // line 1
            for (int i = 0; i < blocks; i++) {
                printf("\u2591");
            }
            printf("\n");
            printf(" %*s \u2502", -1 * spaceForName, current->name);    // line 2
            for (int i = 0; i < blocks; i++) {
                printf("\u2591");
            }
            printf("%d ", current->time);
            printf("\n");
            printf(" %*s \u2502", -1 * spaceForName, ""); // line 3
            for (int i = 0; i < blocks; i++) {
                printf("\u2591");
            }
            printf("\n");
        } else {
            int blocks = current->meetings / blockValue;
            printf(" %*s \u2502", -1 * spaceForName, ""); // line 1
            for (int i = 0; i < blocks; i++) {
                printf("\u2591");
            }
            printf("\n");
            printf(" %*s \u2502", -1 * spaceForName, current->name);    // line 2
            for (int i = 0; i < blocks; i++) {
                printf("\u2591");
            }
            printf("%d", current->meetings);
            printf("\n");
            printf(" %*s \u2502", -1 * spaceForName, ""); // line 3
            for (int i = 0; i < blocks; i++) {
                printf("\u2591");
            }
            printf("\n");
        }

        printf(" %*s \u2502\n", -1 * spaceForName, ""); // spacing line
    }

    printf(" %*s \u2514", -1 * spaceForName, "");
    for (int i = 0; i < remainingSpace + spaceForValue; i++) {
        printf("\u2500");
    }
    printf("\n");
}

void printInstructionMessage() {
    printf("usage: %s [-l length] [-m | -t | -p] [--scaled] filename1 filename2 ..\n", programName);
}

// function to get the maximum name length in a linked list
int getMaxLengthOfNames(meetingHost_t * sortedList) {
    int maxLength = 0;
    for (meetingHost_t * current = sortedList; current != NULL; current = current->next) {
        if (strlen(current->name) > maxLength) {
            maxLength = strlen(current->name);
        }
    }
    return maxLength;
}

// function to get the maximum value of a sorted linked list in the selected mode
int getMaxValueOfSortedList(meetingHost_t *sortedList, int mode) {
    if (mode == MEETINGS) {
        return sortedList->meetings;
    } else if (mode == PARTICIPANTS) {
        return sortedList->participants;
    } else if (mode == TIME) {
        return sortedList->time;
    } else {
        return sortedList->meetings;
    }
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

// function to get the total of selected mode in a linked list
int getTotalByMode(meetingHost_t *sortedList, int mode) {
    switch (mode) {
        case MEETINGS:
            return getTotalMeetings(sortedList);
        case PARTICIPANTS:
            return getTotalParticipants(sortedList);
        case TIME:
            return getTotalTime(sortedList);
        default:
            return getTotalMeetings(sortedList);
    }
}

// function to get the total number of meetings in a meetingHost_t linked list
int getTotalMeetings(meetingHost_t *sortedList) {
    int total = 0;
    for (meetingHost_t *current = sortedList; current != NULL; current = current->next) {
        total += current->meetings;
    }
    return total;
}

// function to get the total number of participants in a meetingHost_t linked list
int getTotalParticipants(meetingHost_t *sortedList) {
    int total = 0;
    for (meetingHost_t *current = sortedList; current != NULL; current = current->next) {
        total += current->participants;
    }
    return total;
}

// function to get the total time in a meetingHost_t linked list
int getTotalTime(meetingHost_t *sortedList) {
    int total = 0;
    for (meetingHost_t *current = sortedList; current != NULL; current = current->next) {
        total += current->time;
    }
    return total;
}

// function check the -l argument input and set the required row count
void setRowCount(char *rowString) {
    for (int i = 0; rowString[i] != '\0'; i++) {
        if (isdigit(rowString[i]) == FALSE &&
            !(i == 0 && rowString[1] != '\0' && (rowString[i] == '+' || rowString[i] == '-'))) {
            printf("Invalid options for [-l]\n");
            printInstructionMessage();
            exit(0);
        }
    }
    int length = atoi(rowString);
    if (length < 0) {
        printf("Invalid option(negative) for [-l]\n");
        printInstructionMessage();
        exit(0);
    } else if (length == 0) {
        exit(0);
    } else {
        rows = length;
    }
}

// function to sort the meetingHost_t linked list in the selected mode
meetingHost_t *getSortedListByMode(int mode) {
    switch (mode) {
        case MEETINGS:
            return sortListByMeetings();
        case PARTICIPANTS:
            return sortListByParticipants();
        case TIME:
            return sortListByTime();
        default:
            return sortListByMeetings();
    }
}

// function to sort the meetingHost_t linked list by meeting count
meetingHost_t *sortListByMeetings() {
    meetingHost_t *sortedList = NULL;
    meetingHost_t *sortedListHead = NULL;
    for (int i = 0; meetingHosts != NULL && i < rows; i++) {

        meetingHost_t *tempHighest = meetingHosts, *previousToHighest = NULL, *previous = NULL;
        for (meetingHost_t *current = meetingHosts; current != NULL; previous = current, current = current->next) {
            if (tempHighest->meetings <= current->meetings) {
                tempHighest = current;
                previousToHighest = previous;
            }
        }

        if (sortedList == NULL) {
            sortedList = tempHighest;
            sortedListHead = sortedList;
        } else {
            sortedListHead->next = tempHighest;
            sortedListHead = sortedListHead->next;
        }

        if (previousToHighest == NULL) {
            meetingHosts = tempHighest->next;
        } else {
            previousToHighest->next = tempHighest->next;
        }

    }
    sortedListHead->next = NULL;
    return sortedList;
}

// function to sort the meetingHost_t linked list by participant count
meetingHost_t *sortListByParticipants() {
    meetingHost_t *sortedList = NULL;
    meetingHost_t *sortedListHead = NULL;
    for (int i = 0; meetingHosts != NULL && i < rows; i++) {

        meetingHost_t *tempHighest = meetingHosts, *previousToHighest = NULL, *previous = NULL;
        for (meetingHost_t *current = meetingHosts; current != NULL; previous = current, current = current->next) {
            if (tempHighest->participants <= current->participants) {
                tempHighest = current;
                previousToHighest = previous;
            }
        }

        if (sortedList == NULL) {
            sortedList = tempHighest;
            sortedListHead = sortedList;
        } else {
            sortedListHead->next = tempHighest;
            sortedListHead = sortedListHead->next;
        }

        if (previousToHighest == NULL) {
            meetingHosts = tempHighest->next;
        } else {
            previousToHighest->next = tempHighest->next;
        }

    }
    sortedListHead->next = NULL;
    return sortedList;
}

// function to sort the meetingHost_t linked list by time duration
meetingHost_t *sortListByTime() {
    meetingHost_t *sortedList = NULL;
    meetingHost_t *sortedListHead = NULL;
    for (int i = 0; meetingHosts != NULL && i < rows; i++) {

        meetingHost_t *tempHighest = meetingHosts, *previousToHighest = NULL, *previous = NULL;
        for (meetingHost_t *current = meetingHosts; current != NULL; previous = current, current = current->next) {
            if (tempHighest->time <= current->time) {
                tempHighest = current;
                previousToHighest = previous;
            }
        }

        if (sortedList == NULL) {
            sortedList = tempHighest;
            sortedListHead = sortedList;
        } else {
            sortedListHead->next = tempHighest;
            sortedListHead = sortedListHead->next;
        }

        if (previousToHighest == NULL) {
            meetingHosts = tempHighest->next;
        } else {
            previousToHighest->next = tempHighest->next;
        }

    }
    sortedListHead->next = NULL;
    return sortedList;
}
