#include "meta.h"
#include "action_stream.h"
#include <ctype.h>
#include <string.h>

void metaComma(ActionStream *a, Match *m, char order)  {
    if (order == PRE)   {
        metaAttach(a, m, order);
        a->ci.outputIndex[0] = m->leftMatchString[1];
        a->ci.outputIndex++;
    }   else    {
        a->ci.spaceNext = true;
    }
}


void metaStop(ActionStream *a, Match *m, char order)    {
    if (order == PRE)   {
        metaAttach(a, m, order);
        a->ci.outputIndex[0] = m->leftMatchString[1];
        a->ci.capNext = true;
        a->ci.outputIndex++;
    }   else    {
        a->ci.spaceNext = true;
    }

}

void metaCase(ActionStream *a, Match *m, char order) {
    if (order == PRE)   {
        metaAttach(a, m, order);
        if (m->arg == CAP_FIRST_WORD)  {
            a->ci.capNext = true;
            a->ci.lowerNext = false;

        }   else if (m->arg == LOWER_FIRST_CHAR)   {
            a->ci.capNext = false;
            a->ci.lowerNext = true;  

        }   else    {
            a->ci.capWord = true;
            a->ci.finishedCapWord = false;
        }        
    }

}



void findPreviousWordStart(ActionStream *a)   {
    for (; a->ci.outputIndex >= a->output && a->ci.outputIndex[0] == '\0'; a->ci.outputIndex--)    {} 
    for (; a->ci.outputIndex >= a->output && !isalpha(*(a->ci.outputIndex)); a->ci.outputIndex--)    {} // scan through any non text
    for (; a->ci.outputIndex >= a->output && isalpha(*(a->ci.outputIndex)); a->ci.outputIndex--)    {} // scan through any text
}

void metaRetroCase(ActionStream *a, Match *m, char order)    {
    char *oldactionsOutputIndex = a->ci.actionsOutputIndex;
    char *oldoutputIndex = a->ci.outputIndex;
    findPreviousWordStart(a);
    metaCase(a, m, order);
    a->ci.actionsOutputIndex = a->ci.outputIndex;
    while ((a->ci.outputIndex < oldoutputIndex))    {
        outputOnce(a);
    }
    a->ci.actionsOutputIndex = oldactionsOutputIndex;
    a->ci.outputIndex = oldoutputIndex;
    
}

char *getPrevTranslation(ActionStream *a)  {
    return a->ci.actionsIndex->nextAction->translation;
}


void metaGlue(ActionStream *a, Match *m, char order) {

    if (m->arg & PRE && a->ci.glue)  {
        uint8_t *oldactionsOutputIndex = a->ci.actionsOutputIndex;
        a->ci.actionsOutputIndex = a->ci.glue;
        skipPrefix(a, a->spaceString);
        if (prefix("{&", a->ci.actionsOutputIndex))  {
            a->ci.outputIndex -= strlen(a->spaceString); // 
        }
        a->ci.actionsOutputIndex = oldactionsOutputIndex;
    }   else if (m->arg & POST) {
        a->ci.glue = a->ci.actionsOutputIndex;
    }
}

void metaAttach(ActionStream *a, Match *m, char order)   {
    if (order == PRE && m->arg != ATTACH_RIGHT )   {
        if (prefix(a->ci.outputIndex - strlen(a->spaceString), a->spaceString))   {
            a->ci.outputIndex -= strlen(a->spaceString);
        }
        
    }   else if (order == POST && m->arg != ATTACH_LEFT)   {
        a->ci.attachNext = true;
    }
}

void carryCap(ActionStream *a, Match *m, char order)    {

}

void metaRestart(ActionStream *a, Match *m, char order)
{
    a->ci.attachNext = false;
    a->ci.capNext = false;
    a->ci.capWord = false;
    a->ci.finishedCapWord = false;
    a->ci.glue = false;    
}
