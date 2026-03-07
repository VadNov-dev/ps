#include "ps.h"
#include <stddef.h>

typedef void(*printFunc)(const proc* pl, int width);


typedef struct {
    const char* title;
    int width;
    int enable;
    int aligment;
    printFunc func;
} column;

