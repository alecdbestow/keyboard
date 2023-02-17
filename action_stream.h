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

typedef struct compilerIndex
{
    size_t index;
    uint8_t *outIndex;
    uint8_t *actionIndex;

    bool inCommand;

    bool capWord;
    bool lowerNext;
    bool capNext;
    bool attachNext;
    bool finishedCapWord;
    uint8_t *glue; 
    
} CompilerIndex;



typedef struct action
{
    Stroke stroke;
    uint8_t *translation;
    size_t length;
} Action;


typedef struct action_stream
{
    size_t end;
    Action actions[MAX_STORED_ACTIONS_LENGTH];
    uint8_t actionsOutput[MAX_OUTPUT_LENGTH];
    uint8_t output[MAX_OUTPUT_LENGTH];
    Dictionary d;
    char spaceString[MAX_TRANSLATION_LENGTH];
    CompilerIndex ci; 
    
} ActionStream;

typedef void (*Func)(ActionStream *, char);

typedef struct compileMatch
{
    Func func;
    char arg;
    char match[10];
    uint8_t order;
    uint8_t prefixPos;
} CompileMatch;



static size_t lstrcpy(char *dest, const char *source);
void initActionStream(ActionStream *a);
size_t getBounded(ActionStream *a, int i);
void getCombinedStrokes(ActionStream *a, char* strokes, size_t start);


void addStroke(ActionStream *a, Stroke stroke);


//void compileActions(ActionStream *a);
void compileOutput(ActionStream *a);
void outputCommand(ActionStream *a);
void callMatchFunc(ActionStream *a, CompileMatch *match, uint8_t order);
bool outputOnce(ActionStream *a, bool checkIndex);
void skipPrefix(ActionStream *a, uint8_t *match);
CompileMatch *findMatch(ActionStream *a);
bool prefix(const char *pre, const char *str);



#endif