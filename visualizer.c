#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define FALSE 0
#define TRUE 1

#define MEETINGS 0
#define PARTICIPANTS 1
#define TIME 2

struct _ {
    int meetings;
    char *name;
    int participants;
    int time;
    struct _ *next;
} typedef meetingHost_t;

int getMaxLengthOfNames(meetingHost_t *);
int getMaxValue(meetingHost_t *, int);
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

meetingHost_t *meetingHosts = NULL;
int rows = 10;

int main(int argc, char ** argv) {
    int mode;
    int isScaled = FALSE;
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
        } else if (0 == strcmp(argv[argIndex], "--scaled")) {
            isScaled = TRUE;
        } else if (0 == strcmp(argv[argIndex], "-l")) {
            if (argIndex == argc - 1) {
                // print length not given error
                return 0;
            } else {
                argIndex++;
                setRowCount(argv[argIndex]);
            }
        } else {
            // terminate immediately if not .csv format
            int length = strlen(argv[argIndex]);
            char *extension = &(argv[argIndex])[length - 4];
            if (0 != strcmp(extension, ".csv")) {
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

    for (int i = 0; i < fileNamesIndex; i++) {
        filePointer = NULL;
        filePointer = fopen(*fileNames + i, "r");
        if (filePointer == NULL) {
            // terminate immediately
            return 0;
        } else {
            char name[255];
            char participants[50];
            char time[50];
            while (TRUE) {
                int scanRes = fscanf(filePointer, "%[^,],%[^,],%[^\n]\n", name, participants, time);
                if (scanRes == EOF) {
                    break;
                } else if (scanRes != 3) {
                    // error
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

    int total = getTotalByMode(meetingHosts, mode);
    free(meetingHosts);
    meetingHost_t *sortedList = getSortedListByMode(mode);
    // plot the graph according to the mode
    int spaceForName = getMaxLengthOfNames(sortedList);
    int spaceForValue = getNumberOfDigits(getMaxValue(sortedList, mode));
    int remainingSpace = 80 - spaceForName - spaceForValue - 3;

    float blockValue;
    if (isScaled) {
        blockValue = 1.0 * getMaxValue(sortedList, mode) / remainingSpace;
    } else {
        blockValue = 1.0 * total / remainingSpace;
    }

    printf("\n");

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

int getMaxLengthOfNames(meetingHost_t * sortedList) {
    int maxLength = 0;
    for (meetingHost_t * current = sortedList; current != NULL; current = current->next) {
        if (strlen(current->name) > maxLength) {
            maxLength = strlen(current->name);
        }
    }
    return maxLength;
}

int getMaxValue(meetingHost_t *sortedList, int mode) {
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

int getNumberOfDigits(int number) {
    int digits = 0;
    while (number != 0) {
        number = number / 10;
        digits++;
    }
    return digits;
}

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

int getTotalMeetings(meetingHost_t *sortedList) {
    int total = 0;
    for (meetingHost_t *current = sortedList; current != NULL; current = current->next) {
        total += current->meetings;
    }
    return total;
}

int getTotalParticipants(meetingHost_t *sortedList) {
    int total = 0;
    for (meetingHost_t *current = sortedList; current != NULL; current = current->next) {
        total += current->participants;
    }
    return total;
}

int getTotalTime(meetingHost_t *sortedList) {
    int total = 0;
    for (meetingHost_t *current = sortedList; current != NULL; current = current->next) {
        total += current->time;
    }
    return total;
}

void setRowCount(char *rowString) {
    for (int i = 0; rowString[i] != '\0'; i++) {
        if (isdigit(rowString[i]) == FALSE &&
            !(i == 0 && rowString[1] != '\0' && (rowString[i] == '+' || rowString[i] == '-'))) {
            // print invalid options for length error
            exit(0);
        }
    }
    int length = atoi(rowString);
    if (length < 0) {
        // print minus length error
        exit(0);
    } else if (length == 0) {
        exit(0);
    } else {
        rows = length;
    }
}

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
