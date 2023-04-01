#ifndef _META_H_
#define _META_H_

#include "formatter.h"
#include "regex/regex.h"

#define NUM_MATCHES 21
#define MAX_MATCH_LENGTH 100

typedef void (*Func)(Action *a, ActionHandler *ah);
void MetaComma();


typedef struct meta {
    Regex match;
    char matchString[MAX_MATCH_LENGTH];
    Func matchFunction;
} Meta;

typedef Meta Metas[NUM_MATCHES];

void MetasInit(Metas *m);
Func MetasGet(Metas *m, Action *a);



#endif