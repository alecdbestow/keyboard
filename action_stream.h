#ifndef _ACTION_STREAM_H_
#define _ACTION_STREAM_H_

#include "meta.h"
#include "stroke.h"
#include "dictionary.h"


#define MAX_STORED_ACTIONS_LENGTH 20
#define MAX_TRANSLATION_LENGTH 100
#define MAX_OUTPUT_LENGTH MAX_STORED_ACTIONS_LENGTH * MAX_TRANSLATION_LENGTH
#define MAX_NUM_STROKES 11

#define POST 1
#define PRE 1<<1


typedef struct match
{
    char arg;
    char leftMatchString[10];
    char rightMatchString[10];
    char *start;
    char *end;
} Match;

typedef struct action
{
    Stroke stroke;
    char *translation;
    size_t length;
    struct action *nextAction;
    struct action *prevAction;
} Action;

typedef struct compilerIndex
{
    Action *actionsIndex;
    char *outputIndex;
    char *actionsOutputIndex;

    bool inCommand;

    bool capWord;
    bool lowerNext;
    bool capNext;
    bool attachNext;
    bool finishedCapWord;
    uint8_t *glue; 
    
} CompilerIndex;


typedef struct action_stream
{
    Action *end;
    Action actions[MAX_STORED_ACTIONS_LENGTH];
    char actionsOutput[MAX_OUTPUT_LENGTH];
    char outputOld[MAX_OUTPUT_LENGTH];
    char output[MAX_OUTPUT_LENGTH];
    Dictionary d;
    char spaceString[MAX_TRANSLATION_LENGTH];
    CompilerIndex ci; 
    
} ActionStream;


typedef void (*Func)(ActionStream *, Match *, char);

typedef struct commandMatch  {
    Func func;
    Match match;
} CommandMatch;


static size_t lstrcpy(char *dest, const char *source);
void ActionStreamInit(ActionStream *a);
void ActionStreamGetCombinedStrokes(ActionStream *a, char* strokes, Action* start);
void ActionStreamAddStroke(ActionStream *a, Stroke stroke);
void ActionStreamUndo(ActionStream *a);
void addString(ActionStream *a, Action *index, uint8_t *trans);
void ActionStreamCompileOutput(ActionStream *a);
void ActionStreamCommandOutput(ActionStream *a);
void outputOnce(ActionStream *a);
bool inIndex(ActionStream *a, char *pos);
Action *ActionStreamGetNextTranslation(ActionStream *a, Action *index);
void skipPrefix(ActionStream *a, char *match);
bool prefix(const char *pre, const char *str);
void ActionStreamWipeTranslations(ActionStream *a, Action *index);
Action* ActionStreamSearchForTranslation(ActionStream *a, Action *index);
void ActionStreamSearchForStroke(ActionStream *a, Action *index, Stroke stroke);



#endif