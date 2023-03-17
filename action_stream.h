#ifndef _ACTION_STREAM_H_
#define _ACTION_STREAM_H_

#include "meta.h"
#include "stroke.h"
#include "dictionary.h"
#include "action.h"
#include "outputter.h"


#define MAX_STORED_ACTIONS_LENGTH 20
#define MAX_OUTPUT_LENGTH MAX_STORED_ACTIONS_LENGTH * MAX_TRANSLATION_LENGTH
#define MAX_NUM_STROKES 11

typedef struct action_stream
{
    Action actions[MAX_STORED_ACTIONS_LENGTH];
    Dictionary d;
    char outputOld[MAX_OUTPUT_LENGTH];
    char output[MAX_OUTPUT_LENGTH];

    
} ActionStream;

void ActionStreamInit(ActionStream *a);
void ActionStreamGetCombinedStrokes(ActionStream *a, char* strokes, uint start);
void ActionStreamAddStroke(ActionStream *a, Stroke stroke);
void ActionStreamUndo(ActionStream *a);
void ActionStreamCompileOutput(ActionStream *a, char *output);
uint ActionStreamGetNextTranslation(ActionStream *a, uint index);
void ActionStreamWipeTranslations(ActionStream *a, uint index);
void ActionStreamSearchForStroke(ActionStream *a, uint index);



#endif