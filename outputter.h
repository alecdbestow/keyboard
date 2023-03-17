#ifndef _OUTPUTTER_H_
#define _OUTPUTTER_H_

#include "action.h"
#include "match.h"

typedef struct inOut {
    char *input;
    char *output;
}   InOut;

typedef struct outputter    {
    Translation spaceString;
    bool capNext;
    bool capWord;
    bool spaceNext;
    bool attachNext;
    bool attachPrev;
    bool lowerNext;
    bool glue;
    InOut inOut;
} Outputter;

InOut OutputterOutputMatch(Outputter *o, InOut inOut, Match *m);
InOut OutputterOutputCommand(Outputter *o, InOut inOut);
InOut OutputterOutputAlpha(Outputter *o, InOut inOut);
InOut OutputterOutputSpace(Outputter *o, InOut inOut);
InOut OutputterOutputOnce(Outputter *o, InOut inOut);
void OutputterOutputNull(Outputter *o, InOut inOut);
void OutputterCompileAction(Outputter *o, Action *a);
void  OutputterPreCompileAction(Outputter *o, Action *a);
void  OutputterInit(Outputter *o);

#endif