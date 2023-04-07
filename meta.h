#ifndef _META_H_
#define _META_H_

#include "regex/regex.h"

#define NUM_MATCHES 13
#define MAX_MATCH_LENGTH 100

typedef struct action Action;
typedef struct actionHandler ActionHandler;

typedef void (*Func)(Action *a, ActionHandler *ah);

void MetaComma(Action *a, ActionHandler *ah);
void MetaStop(Action *a, ActionHandler *ah);
void MetaCapFirstWord(Action *a, ActionHandler *ah);
void MetaLowerFirstChar(Action *a, ActionHandler *ah);
void MetaUpperFirstWord(Action *a, ActionHandler *ah);
void MetaRetroCapFirstWord(Action *a, ActionHandler *ah);
void MetaRetroLowerFirstChar(Action *a, ActionHandler *ah);
void MetaRetroUpperFirstWord(Action *a, ActionHandler *ah);
void MetaGlue(Action *a, ActionHandler *ah);
void MetaPrevAttach(Action *a, ActionHandler *ah);
void MetaBothAttachOrthography(Action *a, ActionHandler *ah);
void MetaPrevAttachOrthography(Action *a, ActionHandler *ah);
void MetaNextAttachOrthography(Action *a, ActionHandler *ah);

typedef struct meta {
    Regex match;
    char matchString[MAX_MATCH_LENGTH];
    Func matchFunction;
} Meta;

typedef Meta Metas[NUM_MATCHES];

void MetasInit();
Func MetasGet(Action *a);



#endif
