#ifndef _H_FORMATTER_
#define _H_FORMATTER_

#include "translation.h"
#include "regex/regex.h"

#define MAX_NUM_ACTIONS 60
#define MAX_TEXT_LENGTH 100

typedef struct context  {

} Context;

typedef struct action   {
    Context context;
    
    char text[MAX_TEXT_LENGTH];
    char output[MAX_TEXT_LENGTH];
} Action;

typedef struct actionHandler    {
    Action *action;
    Action *beginning;
} ActionHandler;

typedef struct formatter    {
    Action actions[MAX_NUM_ACTIONS];
    Regex atom;
    Regex word;
} Formatter;

void FormatterDo(Formatter *f, Translation *translations);
void ActionFormat(Action *a, ActionHandler *ah);
void ActionFormatCommand(Action *a, ActionHandler *ah);
void ActionFormatWords(Action *a);
void ActionFormatterInit(Action *actionList);
ActionHandler *ActionHandlerGet(ActionHandler *ah, Action *a);
Action *ActionHandlerNextAction(ActionHandler *ah);
Action *ActionHandlerPrevAction(ActionHandler *ah);

#endif