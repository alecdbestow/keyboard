#ifndef _STROKE_STREAM_H_
#define _STROKE_STREAM_H_

#include "meta.h"
#include "stroke.h"
#include "dictionary.h"
#include "translation.h"
#include "outputter.h"


#define MAX_STORED_ACTIONS_LENGTH 20
#define MAX_OUTPUT_LENGTH MAX_STORED_ACTIONS_LENGTH * MAX_TRANSLATION_LENGTH
#define MAX_NUM_STROKES 11

typedef struct stroke_stream
{
    Translation translations[MAX_STORED_ACTIONS_LENGTH];
    Dictionary d;
    char outputOld[MAX_OUTPUT_LENGTH];
    char output[MAX_OUTPUT_LENGTH];

    
} StrokeStream;

void StrokeStreamInit(StrokeStream *a);
void StrokeStreamGetCombinedStrokes(StrokeStream *a, char* strokes, uint start);
void StrokeStreamAddStroke(StrokeStream *a, Stroke stroke);
void StrokeStreamUndo(StrokeStream *a);
void StrokeStreamCompileOutput(StrokeStream *a, char *output);
uint StrokeStreamGetNextTranslation(StrokeStream *a, uint index);
void StrokeStreamWipeTranslations(StrokeStream *a, uint index);
void StrokeStreamSearchForStroke(StrokeStream *a, uint index);



#endif