#include <stdlib.h>
#include <locale.h>
#include "ps.h"

int getAvailableProcs(procList *pl, options* opt);
void sortAvailableProcs(procList* pl, options* opt);
void printTable(const procList* pl, options* opt);
void parseArgs(int argc, char **argv, options* opt);


int main(int argc, char **argv) {
    setlocale(LC_ALL, "");
    options opt = {.sortMode = NOT_SORTED, .flags = 0, .limits = 0};
    procList psList = {.ps = NULL, .capacity = 0, .size = 0};
    parseArgs(argc, argv, &opt);
    getAvailableProcs(&psList, &opt);
    sortAvailableProcs(&psList, &opt);
    printTable(&psList, &opt);
    free(psList.ps);
    return 0;
}
