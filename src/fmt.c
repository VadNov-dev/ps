#include <getopt.h>
#include <err.h>
#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fmt.h"



void parseArgs(int argc, char **argv, options* opt) {
    int args;

    while ((args = getopt(argc, argv, "asN:")) != -1) {
        switch (args) {
        case 'N':
        {
            errno = 0;
            char *end;
            opt->flags |= STRING_RESTRICTION;
            opt->limits = strtol(optarg, &end, 10);
            if (errno == ERANGE) {
                errx(2, "Number out of range");
            }
            if (*end != '\0' || opt->limits <=0) {
                errx(2, "Bad command line args");
            }
            break;
        }
        case 'a':
        {
            opt->flags |= SHOW_ALL;
            break;
        }
        case 's':
        {
            opt->sortMode = SORT_BY_NAME;
            break;
        }
        default:
            errx(2, "Bad command line args");
        }
    }
}



static void printPid(const proc* pl, int width) {
    printf("%*d", width, pl->pid);
}

static void printState(const proc* pl, int  width) {
    printf("%*c", width, pl->state);
}

static void printRss(const proc* pl, int width) {
    printf("%*" PRId64, width, pl->rss);
}

static void printVsize(const proc* pl, int width) {
    printf("%*" PRIu64, width, pl->vsize);
}

static void printMemPercent(const proc* pl, int width) {
    printf("%*.1f", width, pl->memoryPercent);
}

static void printCpuPercent(const proc* pl, int width) {
    printf("%*.1f", width, pl->cpuPercent);
}

static void printComm(const proc* pl, int width) {
    printf("%-*s", width, pl->comm);
}

static int digits_u64(uint64_t v) {
    int n = 1;
    while (v >= 10) {
        v /= 10;
        n++;
    }
    return n;
}

static int digits_i64(int64_t v) {
    if (v < 0) {
        uint64_t uv = (uint64_t)(-(v + 1)) + 1; // безопаснее для INT64_MIN
        return digits_u64(uv) + 1;
    }
    return digits_u64((uint64_t)v);
}

static int digits_int(int v) {
    if (v < 0) {
        unsigned uv = (unsigned)(-(v + 1)) + 1;
        int n = 1;
        while (uv >= 10) {
            uv /= 10;
            n++;
        }
        return n + 1;
    }
    int n = 1;
    while (v >= 10) {
        v /= 10;
        n++;
    }
    return n;
}

void computeWidth(column* cols, int numCols, const procList* pl, int limits) {
    for(int i = 0; i< limits; ++i){
        const proc* ps = &pl->ps[i];

        for(int c = 0; c < numCols; ++c) {
            if(!cols[c].enable)
                continue;
            
            int width = cols[c].width;
            int d = 0;
            switch (c) {
                case 0: // PID
                    d = digits_int(ps->pid);
                    if (d > width) {
                        width = d;
                    }
                    break;
                case 1: // RSS
                    d = digits_i64(ps->rss);
                    if (d > width) {
                        width = d;
                    }
                    break;
                case 2: // VSize
                    d = digits_u64(ps->vsize);
                    if (d > width) {
                        width = d;
                    }
                    break;
                case 3: // State
                    if (1 > width) width = 1;
                    break;
                case 4: // CPU%
                    width = 5; // "100.0"
                    break;
                case 5: // MEM%
                    width = 5; // "100.0"
                    break;
                case 6: // COMM
                    width = (int)strlen(ps->comm);
                    break;
            }
            if (width > cols[c].width) {
                cols[c].width = width;
            }
        }
    }
}

static void printHeader(const column* cols, int numCols) {
    int first = 1;
    for(int i = 0; i < numCols; ++i) {
        if(!cols[i].enable)
            continue;
        
        if(!first) {
            printf(" ");
        }
        first = 0;
        if(cols[i].aligment) {
            printf("%-*s", cols[i].width, cols[i].title);
        } else {
            printf("%*s", cols[i].width, cols[i].title);
        }
    }
}

static void printData(const column* cols, int numCols, const procList* pl, int limits) {
    for(int i = 0; i < limits; ++i) {
        int first = 1;
        const proc* ps = &pl->ps[i];
        for(int c = 0; c < numCols; ++c) {
            if(!cols[c].enable)
                continue;

            if (!first) {
                printf(" ");
            }
            first = 0;

            cols[c].func(ps, cols[c].width);
        }
        printf("\n");
    }
}

void printTable(const procList* pl, options* opt) {
    column cols[] = {
           {"PID", (int)strlen("PID"), 1, 0, printPid},
           {"RSS", (int)strlen("RSS"), 1, 0, printRss},
           {"VSize", (int)strlen("VSize"), (opt->flags & SHOW_ALL) != 0, 0, printVsize},
           {"State", (int)strlen("State"), (opt->flags & SHOW_ALL) != 0, 0, printState},
           {"CPU%", (int)strlen("CPU%"), (opt->flags & SHOW_ALL) != 0, 0, printCpuPercent},
           {"MEM%", (int)strlen("MEM%"), 1, 0, printMemPercent},
           {"COMM", (int)strlen("COMM"), 1, 1, printComm}
    };
    size_t numCols = sizeof(cols) / sizeof(cols[0]);
    int limits = 0;
    if(opt->flags & STRING_RESTRICTION) {
        limits = opt->limits;
    } else {
        limits = pl->size;
    }
    computeWidth(cols, numCols, pl, limits);
    printHeader(cols, numCols);
    printf("\n");
    printData(cols, numCols, pl, limits);
}
