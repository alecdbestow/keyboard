#ifndef _H_FORMATTER_
#define _H_FORMATTER_

#include "translation.h"
#include "regex/regex.h"

#define MAX_NUM_ACTIONS 40
#define MAX_TEXT_LENGTH 80
#define MAX_NUM_FRAGMENTS 200
#define MAX_NUM_WORDS 400
#define MAX_STORED_TRANSLATIONS_LENGTH 20

typedef struct word {
    char *word;
    int length;
} Word;

typedef struct action   {
    int length;
    bool prevAttach;
    bool nextAttach;

    bool capFirstChar;
    bool lowerFirstChar;
    bool upperFirstWord;

    bool capPrevWordFirstChar;
    bool lowerPrevWordFirstChar;
    bool upperPrevWord;

    bool glue;
    bool orthography;
    char text[MAX_TEXT_LENGTH];
    char output[MAX_TEXT_LENGTH];
} Action;

typedef struct actionPos    {
    Action *action;
    Word *wordPtr;
    char *pos;
} ActionPos;

typedef struct actionHandler    {
    Action *action;
    Action *beginning;
    int numActions;
} ActionHandler;

typedef struct formatter    {
    Action actions[MAX_NUM_ACTIONS];
    ActionPos totalActions[MAX_NUM_ACTIONS *2];
    Word words[MAX_NUM_WORDS];
    Action spaceAction;
    Regex atom;
    Regex word;
} Formatter;


void FormatterInit(Formatter *f);

// update the list of actionPos' in formatter and 
// make each one point to the start of the word they're part of
void ActionPosConnectWords(ActionPos *aps, Word *words, int numActions, int numWords);

// input a list of translations and output the plover dictionary output
// this is the main method for producing an output
void FormatterDo(Formatter *f, Translation *translations, char *output);

// splits all the translations into actions
int FormatterUpdateActionsTranslations(Formatter *f, Translation *translations, int numTranslations);

void WordInit(Word *w);

// applies formatting to all actions
void FormatterFormatActions(Action *actions, int numActions);

void ActionInit(Action *a);

// formats an action into output and updates the action properties
void ActionFormat(Action *a, ActionHandler *ah);

// convert the {} commands into action properties and updates the output
// e.g if the action.text is {^asdsf} it will update the prevAttach field
// and will update the output to = "asdf"
void ActionFormatCommand(Action *a, ActionHandler *ah);


void ActionFormatWords(Action *a);

// split a translation into multiple actions
// returns the number of actions split into
int ActionsUpdateTranslation(Formatter *f, Action *actions, Translation *translation);

// reposition the current action
void ActionHandlerSet(ActionHandler *ah, Action *a);

// sets the beginning and end values for the action handler
void ActionHandlerInit(ActionHandler *ah, Action *beginning, int numActions);

// returns the next action or Null if the last action
Action *ActionHandlerNextAction(ActionHandler *ah);

// returns the previous action or null if the first action
Action *ActionHandlerPrevAction(ActionHandler *ah);



#endif