#ifndef _OUTPUTTER_H_
#define _OUTPUTTER_H_

#include "translation.h"
#include "match.h"
#include "regex/regex.h"

typedef struct inOut {
    char *input;
    char *output;
}   InOut;

typedef struct outputter    {
    char spaceString[MAX_TRANSLATION_LENGTH];
    bool capNext;
    bool capWord;
    bool spaceNext;
    bool attachNext;
    bool attachPrev;
    bool lowerNext;
    bool glue;
    InOut inOut;
    Regex atom;
    Regex word;
} Outputter;

InOut OutputterOutputMatch(Outputter *o, InOut inOut, Match *m);
InOut OutputterOutputCommand(Outputter *o, InOut inOut);
InOut OutputterOutputAlpha(Outputter *o, InOut inOut);
InOut OutputterOutputSpace(Outputter *o, InOut inOut);
InOut OutputterOutputOnce(Outputter *o, InOut inOut);
void OutputterOutputNull(Outputter *o, InOut inOut);
void OutputterCompileTranslation(Outputter *o, Translation *a);
void  OutputterPreCompileTranslation(Outputter *o, Translation *a);
void  OutputterInit(Outputter *o);

#endif