#include "meta.h"
#include "formatter.h"
#include <ctype.h>
#include <string.h>


static Regex regex;
static const char *matchStrings[] = {
    "\\{[,:;]\\}",
    "\\{[\\.\\!\\?]\\}",
    "\\{-\\|\\}",
    "\\{>\\}",
    "\\{<\\}",
    "\\{\\*-\\|\\}",
    "\\{\\*>\\}",
    "\\{\\*<\\}",
    "\\{\\&.*\\}", 
    "\\{\\^\\}",
    "\\{\\^.+\\^\\}",
    "\\{\\^.+\\}",
    "\\{.+\\^\\}"
};

static const Func matchFuncs[] =    {
    MetaComma,
    MetaStop,
    MetaCapFirstWord,
    MetaLowerFirstChar,
    MetaUpperFirstWord,
    MetaRetroCapFirstWord,
    MetaRetroLowerFirstChar,
    MetaRetroUpperFirstWord,
    MetaGlue,
    MetaPrevAttach,
    MetaBothAttachOrthography,
    MetaPrevAttachOrthography,
    MetaNextAttachOrthography,
};

void MetaComma(Action *a, ActionHandler *ah)    {
    a->prevAttach = true;
    a->output[0] = a->text[1];
    a->output[1] = '\0';
}

void MetaStop(Action *a, ActionHandler *ah) {
    a->prevAttach = true;
    a->output[0] = a->text[1];
    a->output[1] = '\0';
    Action *nextAction = ActionHandlerNextAction(ah);
    if (nextAction) {
        nextAction->capFirstChar = true;
    }
}

void MetaCapFirstWord(Action *a, ActionHandler *ah) {
    a->capFirstChar = true;
}

void MetaLowerFirstChar(Action *a, ActionHandler *ah)   {
    a->lowerFirstChar = true;
}

void MetaUpperFirstWord(Action *a, ActionHandler *ah)   {
    a->upperFirstWord = true;
}

void MetaRetroCapFirstWord(Action *a, ActionHandler *ah)    {
    a->capPrevWordFirstChar = true;
}

void MetaRetroLowerFirstChar(Action *a, ActionHandler *ah)  {
    a->lowerPrevWordFirstChar = true;
}

void MetaRetroUpperFirstWord(Action *a, ActionHandler *ah)  {
    a->lowerPrevWordFirstChar = true;
}

void MetaGlue(Action *a, ActionHandler *ah) {
    a->glue = true;
    Action *prevAction = ActionHandlerPrevAction(ah);
    if (prevAction && prevAction->glue)   {
        a->prevAttach = true;
    }
    strncpy(a->output, a->text + 2, a->length - 3);
}

void MetaPrevAttach(Action *a, ActionHandler *ah)   { 
    a->prevAttach = true;
    Action *nextAction = ActionHandlerNextAction(ah);
    if (nextAction)   {
        a->prevAttach = true;
    }
}

void MetaBothAttachOrthography(Action *a, ActionHandler *ah)    {
    a->prevAttach = true;
    a->nextAttach = true;
    a->orthography = true;
    strncpy(a->output, a->text + 2, a->length - 4);
    a->output[a->length - 4] = '\0';
}

void MetaPrevAttachOrthography(Action *a, ActionHandler *ah)    {
    a->prevAttach = true;
    a->orthography = true;
    strncpy(a->output, a->text + 2, a->length - 3);
    a->output[a->length - 3] = '\0';
}

void MetaNextAttachOrthography(Action *a, ActionHandler *ah)    {
    a->nextAttach = true;
    a->orthography = true;
    strncpy(a->output, a->text + 1, a->length - 3);
    a->output[a->length - 3] = '\0';
}

void MetasInit()
{

    regex = compileMultiMatchingRegex(NUM_MATCHES, matchStrings);
}

Func MetasGet(Action *a)
{
    int exitNum = -1;
    startsWithRegexN(regex, a->text, MAX_MATCH_LENGTH, &(a->length), &exitNum);
    if (exitNum == -1)  {
        return NULL;
    }   else    {
        return matchFuncs[exitNum];
    }
    
}

