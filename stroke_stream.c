#include "stroke_stream.h"
#include "string_help.h"
#include <string.h>
#include <ctype.h>
//  0  1  2  3  4  5  6
//  S1 S2 S3 S4 S5 S6 S7
//  W        W     W  W


#define ACTIONS_END MAX_STORED_ACTIONS_LENGTH - 1


void StrokeStreamInit(StrokeStream *a)  {
    dictInit(&(a->d), 0, 150000*10);
    for (int i = 0; i < MAX_STORED_ACTIONS_LENGTH; i++) {
        TranslationInit(a->translations + i);
    }
    a->output[0] = '\0';
    a->outputOld[0] = '\0';
    //a->end = ACTIONS_END;
}

//
void StrokeStreamGetCombinedStrokes(StrokeStream *a, char* strokes, uint start)  {
    strcpy(strokes, a->translations[start].stroke);
    for (start++; start < MAX_STORED_ACTIONS_LENGTH; start++)    {
        strcat(strokes, "/");
        strcat(strokes, a->translations[start].stroke);
    }
}

// add a stroke to the translation stream and compile the output
// This is the main input function for the translation stream
void StrokeStreamAddStroke(StrokeStream *a, Stroke stroke)   {

    for (uint i = 0; i < ACTIONS_END; i++)    {
        TranslationCopy(a->translations + i, a->translations + i + 1);
    }
    TranslationInit(a->translations + ACTIONS_END);
    strcpy(a->translations[ACTIONS_END].stroke, stroke);
        
    // compile the output and save it to oldOutput
    // needed to calculate the difference between the strings for typing
    StrokeStreamCompileOutput(a, a->outputOld);

    uint index = MAX_STORED_ACTIONS_LENGTH - MAX_NUM_STROKES;
    // start on an translation with a translation
    index = StrokeStreamGetNextTranslation(a, index);
 
    StrokeStreamSearchForStroke(a, index);
    // compile with the new stroke added
    StrokeStreamCompileOutput(a, a->output);

}
/*
void StrokeStreamUndo(StrokeStream * a)
{
    Translation *index = a->end;

    Stroke history[MAX_NUM_STROKES] = {0};
    uint numStrokes = 0;

    StrokeStreamCompileOutput(a);
    strcpy(a->outputOld, a->output);

    while (numStrokes < MAX_NUM_STROKES) {
        a->end = a->end->prevTranslation;
        strcpy(history[numStrokes], index->stroke);
        index->stroke[0] = '\0';
        index->translation[0] = '\0';
        if (index->length > 0) {
            index->length = 0;
            break;
        }
        numStrokes++;
        index = index->prevTranslation;
        
    }
    while (numStrokes >= 1) {
        a->end = a->end->nextTranslation;
        strcpy(a->end->stroke, history[numStrokes]);
        StrokeStreamSearchForStroke(a, a->end, history[numStrokes]);
        numStrokes--;
    }
    StrokeStreamCompileOutput(a);
}
*/
void StrokeStreamSearchForStroke(StrokeStream *a, uint index) {
    //check all the strokes for translation
    char translation[MAX_TRANSLATION_LENGTH];
    char combinedStrokes[NUM_STENO_CHARS * MAX_NUM_STROKES];
    while (index < MAX_STORED_ACTIONS_LENGTH)   {
        if (a->translations[index].translation[0] != '\0' || index == ACTIONS_END)  {

            StrokeStreamGetCombinedStrokes(a, combinedStrokes, index);
            getTranslation(&(a->d), combinedStrokes, translation);
            
            if (translation[0] != '\0')   {
                strcpy(a->translations[index].translation, translation);
                StrokeStreamWipeTranslations(a, index + 1);
                return;
            }
        }
        index++;
    }
    // if no translations are found, then output the stroke instead
    strcpy(a->translations[ACTIONS_END].translation, a->translations[ACTIONS_END].stroke);
}

uint StrokeStreamGetNextTranslation(StrokeStream *a, uint index) {
    while (index < ACTIONS_END) {
        if (a->translations[index].translation[0] != '\0') {
            break;
        }
        index++;
    }
    return index;
}

void StrokeStreamWipeTranslations(StrokeStream *a, uint index) {
    while (index < MAX_STORED_ACTIONS_LENGTH)   {
        // remove all the translations following the one created as they have now been encorporated
        TranslationRemoveTranslation(a->translations + index);
        index++;
    }
}

void StrokeStreamCompileOutput(StrokeStream *a, char *output)   {
    Outputter o;
    o.attachPrev = false;
    OutputterInit(&o);
    uint i = 0;
    output[0] = '\0';

    OutputterCompileTranslation(&o, a->translations);
    strcat(output, a->translations[i].output);

    for (i += 1; i < MAX_STORED_ACTIONS_LENGTH; i++)  {
        if (a->translations[i].translation[0] != '\0')   {
        OutputterPreCompileTranslation(&o, a->translations + i);
        InOut inOut = {o.spaceString, output + strlen(output)};
        inOut = OutputterOutputSpace(&o, inOut);
        OutputterOutputNull(&o, inOut);
        OutputterCompileTranslation(&o, a->translations + i);
        o.attachPrev = false;
        strcat(output, a->translations[i].output);
        }



    }
}