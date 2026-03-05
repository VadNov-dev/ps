#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <inttypes.h>
#include "ps.h"


int cmpByName(const void *a, const void *b) {
    const proc *pa = a;
    const proc *pb = b;
    return strcoll(pa->comm, pb->comm);
}

int cmpByPid(const void *a, const void *b) {
    const proc *pa = a;
    const proc *pb = b;
    return (pa->pid > pb->pid) - (pa->pid < pb->pid);
}

int cmpByVmRssUp(const void *a, const void *b) {
    const proc *pa = a;
    const proc *pb = b;
    return (pa->rss > pb->rss) - (pa->rss < pb->rss);
}

int cmpByVmRssDown(const void *a, const void *b) {
    const proc *pa = a;
    const proc *pb = b;
    return (pa->rss < pb->rss) - (pa->rss > pb->rss);
}

int cmpByVmSizeUp(const void *a, const void *b) {
    const proc *pa = a;
    const proc *pb = b;
    return (pa->vsize > pb->vsize) - (pa->vsize < pb->vsize);
}

int cmpByVmSizeDown(const void *a, const void *b) {
    const proc *pa = a;
    const proc *pb = b;
    return (pa->vsize < pb->vsize) - (pa->vsize > pb->vsize);
}

int cmpByMemoryPercentUp(const void *a, const void *b) {
    const proc *pa = a;
    const proc *pb = b;
    return (pa->memoryPercent > pb->memoryPercent) - (pa->memoryPercent < pb->memoryPercent);
}

int cmpByMemoryPercentDown(const void *a, const void *b) {
    const proc *pa = a;
    const proc *pb = b;
    return (pa->memoryPercent < pb->memoryPercent) - (pa->memoryPercent > pb->memoryPercent);
}

void sort(procList* pl, options* opt) {
    switch (opt->sortMode) {
    case SORT_BY_NAME:
        qsort(pl->ps, pl->size, sizeof(proc), cmpByName);
        break;
    case SORT_BY_PID:
        qsort(pl->ps, pl->size, sizeof(proc), cmpByPid);
        break;
    case SORT_UP_BY_VM_RSS:
        qsort(pl->ps, pl->size, sizeof(proc), cmpByVmRssUp);
        break;
    case SORT_DOWN_BY_VM_RSS:
        qsort(pl->ps, pl->size, sizeof(proc), cmpByVmRssDown);
        break;
    case SORT_UP_BY_VM_SIZE:
        qsort(pl->ps, pl->size, sizeof(proc), cmpByVmSizeUp);
        break;
    case SORT_DOWN_BY_VM_SIZE:
        qsort(pl->ps, pl->size, sizeof(proc), cmpByVmSizeDown);
        break;
    case SORT_UP_BY_MEMORY_PERCENT:
        qsort(pl->ps, pl->size, sizeof(proc), cmpByMemoryPercentUp);
        break;
    case SORT_DOWN_BY_MEMORY_PERCENT:
        qsort(pl->ps, pl->size, sizeof(proc), cmpByMemoryPercentDown);
        break;
    case NOT_SORTED:
    default:
        break;
    }
}

int reallocPs(procList *pl) {
    if(pl->size == pl->capacity) {
        int newCapacity = pl->capacity == 0 ? 1 : pl->capacity * 2;
        proc* newPs = realloc(pl->ps, newCapacity * sizeof(proc));
        if(newPs == NULL) {
            return -1;
        }
        memset(newPs + pl->capacity, 0, (newCapacity - pl->capacity) * sizeof(proc));
        pl->ps = newPs;
        pl->capacity = newCapacity;
        return 0;
    }

    return 0;
}


int field_u64(const char *s, uint64_t *out) {
    if (!s || !*s) return -1;
    char *end;
    errno = 0;
    *out = strtoumax(s, &end, 10);
    if (errno || end == s || *end != '\0') return -1;
    return 0;
}

int field_i64(const char *s, int64_t *out) {
    if (!s || !*s) return -1;
    char *end;
    errno = 0;
    *out = strtoimax(s, &end, 10);
    if (errno || end == s || *end != '\0') return -1;
    return 0;
}
