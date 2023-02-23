#include "meta.h"
#include "action_stream.h"
#include <ctype.h>
#include <string.h>

void metaComma(ActionStream *a, char comma)  {
    metaAttach(a, comma);
    *(a->ci.outputIndex) = comma;
    a->ci.outputIndex++;
}


void metaStop(ActionStream *a, char stop)    {
    metaAttach(a, stop);
    *(a->ci.outputIndex) = stop;
    a->ci.capNext = true;
    a->ci.outputIndex++;
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
    for (; a->ci.outputIndex >= a->output && a->ci.outputIndex[0] == '\0'; a->ci.outputIndex--)    {} 
    for (; a->ci.outputIndex >= a->output && !isalpha(*(a->ci.outputIndex)); a->ci.outputIndex--)    {} // scan through any non text
    for (; a->ci.outputIndex >= a->output && isalpha(*(a->ci.outputIndex)); a->ci.outputIndex--)    {} // scan through any text
}

void metaRetroCase(ActionStream *a, char arg)    {
    uint8_t *oldactionsOutputIndex = a->ci.actionsOutputIndex;
    uint8_t *oldoutputIndex = a->ci.outputIndex;
    findPreviousWordStart(a);
    metaCase(a, arg);
    a->ci.actionsOutputIndex = a->ci.outputIndex;
    while ((a->ci.outputIndex < oldoutputIndex) && outputOnce(a, false)){}
    a->ci.actionsOutputIndex = oldactionsOutputIndex;
    a->ci.outputIndex = oldoutputIndex;
    
}

char *getPrevTranslation(ActionStream *a)  {
    return a->ci.actionsIndex->nextAction->translation;
}


void metaGlue(ActionStream *a, char arg) {

    if (arg & PRE && a->ci.glue)  {
        uint8_t *oldactionsOutputIndex = a->ci.actionsOutputIndex;
        a->ci.actionsOutputIndex = a->ci.glue;
        skipPrefix(a, a->spaceString);
        if (prefix("{&", a->ci.actionsOutputIndex))  {
            a->ci.outputIndex -= strlen(a->spaceString); // 
        }
        a->ci.actionsOutputIndex = oldactionsOutputIndex;
    }   else if (arg & POST) {
        a->ci.glue = a->ci.actionsOutputIndex;
    }
}

void metaAttach(ActionStream *a, char arg)   {
    a->ci.outputIndex -= strlen(a->spaceString);
}

void carryCap(ActionStream *a, char arg)    {

}