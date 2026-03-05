#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <unistd.h>
#include <inttypes.h>
#include "ps.h"

int reallocPs(procList *pl);
void sort(procList* pl, options* opt);

static const char *procDir = "/proc";
static const char *psName = "comm";
static const char *status = "stat";
static const char *memInfo = "/proc/meminfo";
static long cpuFreq = 0;
static long pageSize = 0;
static int isCorrectDirectory(char *path, char *name);
static long getTotalMemory();
static int getPsInfo(proc* ps1);
int field_u64(const char *s, uint64_t *out);
int field_i64(const char *s, int64_t *out);

int getPsInfo(proc* ps) {
    char path[256];
    snprintf(path, sizeof(path), "%s/%d/%s", procDir, ps->pid, status);
    FILE* psFile = fopen(path, "r");
    if(psFile == NULL) {
        return -1;
    }
    char buffer[4096];
    if (!fgets(buffer, sizeof(buffer), psFile)) {
        fclose(psFile);
        return -1; 
    }
    fclose(psFile);

    char* commStart = strchr(buffer, '(');
    if (!commStart) {
        return -1;
    }
    commStart++;
    char *commEnd = strrchr(buffer, ')');
    if (!commEnd) {
        return -1;
    }
    size_t commLength = commEnd - commStart;
    if (commLength >= sizeof(ps->comm)) {
        commLength = sizeof(ps->comm) - 1;
    }
    strncpy(ps->comm, commStart, commLength);
    ps->comm[commLength] = '\0';
    char* nextEntry = commEnd + 2;

    char *token[51] = {0};
    int i = 0;
    char *tok = strtok(nextEntry, " \n");
    while (i < 50 && tok) {
        token[i++] = tok;
        tok = strtok(NULL, " \n");
    }

    for (int j = 0; j < 50; j++) {
        if (token[j] == NULL) return -1;
    }

    ps->state = token[0][0];
    if (field_u64(token[11], &ps->utime)     != 0 ||
        field_u64(token[12], &ps->stime)     != 0 ||
        field_u64(token[19], &ps->starttime) != 0 ||
        field_u64(token[20], &ps->vsize)     != 0 ||
        field_i64(token[21], &ps->rss)       != 0) {
        return -1;
    }
    if(ps->utime > 0 && ps->stime > 0 && cpuFreq > 0 && (ps->utime - ps->starttime) != 0) {
        ps->cpuPercent = ((ps->utime + ps->stime) * 100.) / cpuFreq / (ps->utime - ps->starttime/cpuFreq);
    } else {
        ps->cpuPercent = 0;
    }
    ps->rss = ps->rss * pageSize / 1024;
    ps->vsize = ps->vsize / 1024;

    return 0;
}

int getAvailableProcs(procList *pl, options* opt) {
    DIR *dir = opendir(procDir);

    if (!dir) {
        err(1, "Failed to open directory");
    }

    struct dirent *entry;
    pl->size = 0;
    long totalMemory = getTotalMemory();
    cpuFreq = sysconf(_SC_CLK_TCK);
    pageSize = sysconf(_SC_PAGESIZE);
    while ((entry = readdir(dir)) != NULL) {
        char path[512];
        snprintf(path, sizeof(path), "%s/%s", procDir, entry->d_name);
        if(!isCorrectDirectory(path, entry->d_name)){
            continue;
        }
        int res = reallocPs(pl);
        if(res != 0) {
            closedir(dir);
            err(1, "Memory allocation failed");
        }

        pl->ps[pl->size].pid = atoi(entry->d_name);
        getPsInfo(&pl->ps[pl->size]);
        pl->ps[pl->size].memoryPercent = totalMemory > 0 ? (pl->ps[pl->size].rss * 100.) / totalMemory : 0;
        pl->size++;

        if((opt->flags & STRING_RESTRICTION) && opt->sortMode == NOT_SORTED) {
            if(opt->limits == pl->size) {
                break;
            }
        }
    }
    closedir(dir);
    return 0;
}

void sortAvailableProcs(procList* pl, options* opt) {
    if(opt->sortMode == NOT_SORTED) {
        return;
    }
    sort(pl, opt);
}

long getTotalMemory() {
    char line[128];
    long totalMemory = -1;
    FILE* memInfoFile = fopen(memInfo, "r");
    if(memInfoFile == NULL) {
        return -1;
    }
    while(fgets(line, sizeof(line), memInfoFile)) {
        if(sscanf(line, "MemTotal: %ld", &totalMemory) == 1 ) {
            break;
        }
    }
    fclose(memInfoFile);
    return totalMemory;
}


int isCorrectDirectory(char *path, char *name) {
    char *end;
    strtol(name, &end, 10);
    if(*end != '\0' || end == name) {
        return 0;
    }
    struct stat st;
    int res = stat(path, &st);
    if (res != 0) 
        return 0;

    return S_ISDIR(st.st_mode);
}