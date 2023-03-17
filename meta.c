#include "meta.h"
#include <ctype.h>
#include <string.h>
#include "outputter.h"

InOut metaComma(Outputter *o, Match *m, InOut inOut, char order)  {
    if (order == PRE)   {
        inOut.output[0] = inOut.input[1];
        inOut.output++;
    }   else    {
        inOut = metaAttach(o, m, inOut, order);
        o->spaceNext = true;
    }
    return inOut;
}


InOut metaStop(Outputter *o, Match *m, InOut inOut, char order)    {
    if (order == PRE)   {
        inOut.output[0] = inOut.input[1];
        inOut.output++;
    }   else    {
        inOut = metaAttach(o, m, inOut, order);
        o->capNext = true;
    }
    return inOut;
}

InOut metaCase(Outputter *o, Match *m, InOut inOut, char order) {
    if (order == PRE)   {
        inOut = metaAttach(o, m, inOut, order);
        if (m->arg == CAP_FIRST_WORD)  {
            o->capNext = true;
            o->lowerNext = false;

        }   else if (m->arg == LOWER_FIRST_CHAR)   {
            o->capNext = false;
            o->lowerNext = true;  

        }   else    {
            o->capWord = true;
        }        
    }

}

InOut metaGlue(Outputter *o, Match *m, InOut inOut, char order) {
    if (order == POST)  {
        o->glue = true;
    }
}

InOut metaAttach(Outputter *o, Match *m, InOut inOut, char order)   {
    if (order == POST)   {
        if (m->arg == ATTACH_LEFT)  {
            o->attachPrev = true;
        }   else    {
            o->attachNext = true;
        }
        
    }
}

InOut carryCap(Outputter *o, Match *m, InOut inOut, char order)    {

}

InOut metaRestart(Outputter *o, Match *m, InOut inOut, char order)
{
    o->attachNext = false;
    o->capNext = false;
    o->capWord = false;
    o->glue = false;    
}
