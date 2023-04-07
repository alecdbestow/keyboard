#ifndef _STROKE_STREAM_H_
#define _STROKE_STREAM_H_

#include "meta.h"
#include "stroke.h"
#include "dictionary.h"
#include "translation.h"
#include "formatter.h"


#define MAX_STORED_TRANSLATIONS_LENGTH 20
#define MAX_OUTPUT_LENGTH MAX_STORED_TRANSLATIONS_LENGTH * MAX_TRANSLATION_LENGTH
#define MAX_NUM_STROKES 11

typedef struct stroke_stream
{
    Translation translations[MAX_STORED_TRANSLATIONS_LENGTH];
    Dictionary d;
    char outputOld[1000];
    char output[1000];
    Formatter formatter;

    
} StrokeStream;

void StrokeStreamInit(StrokeStream *ss);
void StrokeStreamGetCombinedStrokes(StrokeStream *ss, char* strokes, uint start);
void StrokeStreamAddStroke(StrokeStream *ss, Stroke stroke);
void StrokeStreamUndo(StrokeStream *ss);
void StrokeStreamCompileOutput(StrokeStream *ss, char *output);
uint StrokeStreamGetNextTranslation(StrokeStream *ss, uint index);
void StrokeStreamWipeTranslations(StrokeStream *ss, uint index);
void StrokeStreamSearchForStroke(StrokeStream *ss, uint index);



#endif