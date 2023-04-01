#include "stroke_stream.h"
#include "string_help.h"
#include <string.h>
#include <ctype.h>
//  0  1  2  3  4  5  6
//  S1 S2 S3 S4 S5 S6 S7
//  W        W     W  W


#define ACTIONS_END MAX_STORED_TRANSLATIONS_LENGTH - 1


void StrokeStreamInit(StrokeStream *ss)  {
    dictInit(&(ss->d), 0, 150000*10);
    for (int i = 0; i < MAX_STORED_TRANSLATIONS_LENGTH; i++) {
        TranslationInit(ss->translations + i);
    }
    ss->output[0] = '\0';
    ss->outputOld[0] = '\0';
    //ss->end = ACTIONS_END;
}

//
void StrokeStreamGetCombinedStrokes(StrokeStream *ss, char* strokes, uint start)  {
    strcpy(strokes, ss->translations[start].stroke);
    for (start++; start < MAX_STORED_TRANSLATIONS_LENGTH; start++)    {
        strcat(strokes, "/");
        strcat(strokes, ss->translations[start].stroke);
    }
}

// add a stroke to the translation stream and compile the output
// This is the main input function for the translation stream
void StrokeStreamAddStroke(StrokeStream *ss, Stroke stroke)   {

    for (uint i = 0; i < ACTIONS_END; i++)    {
        TranslationCopy(ss->translations + i, ss->translations + i + 1);
    }
    TranslationInit(ss->translations + ACTIONS_END);
    strcpy(ss->translations[ACTIONS_END].stroke, stroke);
        
    // compile the output and save it to oldOutput
    // needed to calculate the difference between the strings for typing
    StrokeStreamCompileOutput(ss, ss->outputOld);

    uint index = MAX_STORED_TRANSLATIONS_LENGTH - MAX_NUM_STROKES;
    // start on an translation with a translation
    index = StrokeStreamGetNextTranslation(ss, index);
 
    StrokeStreamSearchForStroke(ss, index);
    // compile with the new stroke added
    StrokeStreamCompileOutput(ss, ss->output);

}
/*
void StrokeStreamUndo(StrokeStream * a)
{
    Translation *index = ss->end;

    Stroke history[MAX_NUM_STROKES] = {0};
    uint numStrokes = 0;

    StrokeStreamCompileOutput(ss);
    strcpy(ss->outputOld, ss->output);

    while (numStrokes < MAX_NUM_STROKES) {
        ss->end = ss->end->prevTranslation;
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
        ss->end = ss->end->nextTranslation;
        strcpy(ss->end->stroke, history[numStrokes]);
        StrokeStreamSearchForStroke(ss, ss->end, history[numStrokes]);
        numStrokes--;
    }
    StrokeStreamCompileOutput(ss);
}
*/
void StrokeStreamSearchForStroke(StrokeStream *ss, uint index) {
    //check all the strokes for translation
    char translation[MAX_TRANSLATION_LENGTH];
    char combinedStrokes[NUM_STENO_CHARS * MAX_NUM_STROKES];
    while (index < MAX_STORED_TRANSLATIONS_LENGTH)   {
        if (ss->translations[index].english[0] != '\0' || index == ACTIONS_END)  {

            StrokeStreamGetCombinedStrokes(ss, combinedStrokes, index);
            getTranslation(&(ss->d), combinedStrokes, translation);
            
            if (translation[0] != '\0')   {
                strcpy(ss->translations[index].english, translation);
                StrokeStreamWipeTranslations(ss, index + 1);
                return;
            }
        }
        index++;
    }
    // if no translations are found, then output the stroke instead
    strcpy(ss->translations[ACTIONS_END].english, ss->translations[ACTIONS_END].stroke);
}

uint StrokeStreamGetNextTranslation(StrokeStream *ss, uint index) {
    while (index < ACTIONS_END) {
        if (ss->translations[index].english[0] != '\0') {
            break;
        }
        index++;
    }
    return index;
}

void StrokeStreamWipeTranslations(StrokeStream *ss, uint index) {
    while (index < MAX_STORED_TRANSLATIONS_LENGTH)   {
        // remove all the translations following the one created as they have now been encorporated
        TranslationRemoveTranslation(ss->translations + index);
        index++;
    }
}

void StrokeStreamCompileOutput(StrokeStream *ss, char *output)   {
    Outputter o;
    o.attachPrev = false;
    OutputterInit(&o);
    uint i = 0;
    output[0] = '\0';

    OutputterCompileTranslation(&o, ss->translations);
    strcat(output, ss->translations[i].output);

    for (i += 1; i < MAX_STORED_TRANSLATIONS_LENGTH; i++)  {
        if (ss->translations[i].english[0] != '\0')   {
        OutputterPreCompileTranslation(&o, ss->translations + i);
        InOut inOut = {o.spaceString, output + strlen(output)};
        inOut = OutputterOutputSpace(&o, inOut);
        OutputterOutputNull(&o, inOut);
        OutputterCompileTranslation(&o, ss->translations + i);
        o.attachPrev = false;
        strcat(output, ss->translations[i].output);
        }



    }
}