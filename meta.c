#include "meta.h"
#include "action_stream.h"
#include <ctype.h>
#include <string.h>

void metaComma(ActionStream *a, char comma)  {
    *(a->ci.outIndex) = comma;
    a->ci.outIndex++;
}

void metaStop(ActionStream *a, char stop)    {
    *(a->ci.outIndex) = stop;
    a->ci.capNext = true;
    a->ci.outIndex++;
}

void metaCase(ActionStream *a, char arg) {
    if (arg == CAP_FIRST_WORD)  {
        a->ci.capNext = true;
        a->ci.lowerNext = false;
    }   else if (arg == LOWER_FIRST_CHAR)   {
        a->ci.capNext = false;
        a->ci.lowerNext = true;        
    }   else    {
        a->ci.capWord = true;
        a->ci.finishedCapWord = false;
    }
}



void findPreviousWordStart(ActionStream *a)   {
    for (; a->ci.outIndex >= a->output && a->ci.outIndex[0] == '\0'; a->ci.outIndex--)    {} 
    for (; a->ci.outIndex >= a->output && !isalpha(*(a->ci.outIndex)); a->ci.outIndex--)    {} // scan through any non text
    for (; a->ci.outIndex >= a->output && isalpha(*(a->ci.outIndex)); a->ci.outIndex--)    {} // scan through any text
}

void metaRetroCase(ActionStream *a, char arg)    {
    uint8_t *oldActionIndex = a->ci.actionIndex;
    uint8_t *oldOutIndex = a->ci.outIndex;
    findPreviousWordStart(a);
    metaCase(a, arg);
    a->ci.actionIndex = a->ci.outIndex;
    while ((a->ci.outIndex < oldOutIndex) && outputOnce(a, false)){}
    a->ci.actionIndex = oldActionIndex;
    a->ci.outIndex = oldOutIndex;
    
}

char *getPrevTranslation(ActionStream *a)  {
    return a->actions[getBounded(a, a->ci.index - 1)].translation;
}


void metaGlue(ActionStream *a, char arg) {

    if (arg & PRE && a->ci.glue)  {
        uint8_t *oldActionIndex = a->ci.actionIndex;
        a->ci.actionIndex = a->ci.glue;
        skipPrefix(a, a->spaceString);
        if (prefix("{&", a->ci.actionIndex))  {
            a->ci.outIndex -= strlen(a->spaceString); // 
        }
        a->ci.actionIndex = oldActionIndex;
    }   else if (arg & POST) {
        a->ci.glue = a->ci.actionIndex;
    }
}

void metaAttach(ActionStream *a, char arg)   {
    a->ci.outIndex -= strlen(a->spaceString);
}

void carryCap(ActionStream *a, char arg)    {

}