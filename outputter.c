#include "outputter.h"
#include "string.h"
#include "string_help.h"
#include "ctype.h"

#define COMMAND_START '{'

InOut OutputterOutputMatch(Outputter *o, InOut inOut, Match *m)  {
    inOut.input = m->start + strlen(m->leftMatchString);
    while (inOut.input < m->end)    {
        inOut = OutputterOutputOnce(o, inOut);
    }
    inOut.input = m->end + strlen(m->rightMatchString);
    return inOut;
}

InOut OutputterOutputCommand(Outputter *o, InOut inOut) {
    Match m;
    CommandMatch *cm = CommandMatchFindMatch(inOut.input);
    if (!cm)    {
        inOut.input[0] = '\0';
        inOut.output[0] = '\0';
    }   else    {
        inOut = cm->func(o, &(cm->match), inOut, PRE);
        inOut = OutputterOutputMatch(o, inOut, &(cm->match));
        inOut = cm->func(o, &(cm->match), inOut, POST);
    }
    return inOut;
}

InOut OutputterOutputAlpha(Outputter *o, InOut inOut) {
    if (o->capNext) {
        inOut.output[0] = toupper(inOut.input[0]);
        o->capNext = false;
    }
    else if (o->lowerNext)   {
        inOut.output[0] = tolower(inOut.input[0]);
        o->lowerNext = false;
    }
    else    {
        inOut.output[0] = inOut.input[0];
    }
    inOut.input++;
    inOut.output++;
    return inOut;
}

InOut OutputterOutputSpace(Outputter *o, InOut inOut)   {
    if (o->attachPrev)  {
        o->attachPrev = false;
        inOut.input = skipPrefix(inOut.input, o->spaceString);
    }
    else if (o->attachNext)   {
        o->attachNext = false;
        inOut.input = skipPrefix(inOut.input, o->spaceString);
    }   else    {
        for (uint i = 0; i < strlen(o->spaceString); i++)   {
            inOut = OutputterOutputAlpha(o, inOut);
        }
    }
    o->spaceNext = false;
    return inOut;
}

InOut OutputterOutputOnce(Outputter *o, InOut inOut)   {
    if (inOut.input[0] == '\\') {
        inOut.output[0] = inOut.input[1];
        inOut.input += 2;
        inOut.output ++;
    }   else if (inOut.input[0] == COMMAND_START)   {
        inOut = OutputterOutputCommand(o, inOut);
    }   else if (o->spaceNext || prefix(o->spaceString, inOut.input))   {
        inOut = OutputterOutputSpace(o, inOut);
    }   else    {
        inOut = OutputterOutputAlpha(o, inOut);
    }
    return inOut;

}

void OutputterOutputNull(Outputter *o, InOut inOut) {
    inOut.output[0] = '\0';
}

void OutputterCompileAction(Outputter *o, Action *a)    {
    o->attachPrev = false;
    InOut inOut = {a->translation, a->output};
    while (inOut.input[0] != '\0')   {
        inOut = OutputterOutputOnce(o, inOut);  
    }
    OutputterOutputNull(o, inOut);  // add the \0 character at the end
}

//processes only the first commands
void OutputterPreCompileAction(Outputter *o, Action *a)
{
    bool oldGlue = o->glue;
    o->glue = false;
    InOut inOut = {a->translation, a->output};
    while (inOut.input[0] == COMMAND_START)   {
        inOut = OutputterOutputCommand(o, inOut);  
    }
    if (oldGlue && o->glue)    {
        o->attachPrev = true;
    }    
}

void OutputterInit(Outputter *o) {
    o->spaceString[0] = ' ';
    o->capNext = false;
    o->capWord = false;
    o->spaceNext = false;
    o->attachNext = false;
    o->attachPrev = false;
    o->lowerNext = false;
    o->glue = false;
}