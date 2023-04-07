#include "stroke_stream.h"
#include "string_help.h"
#include <string.h>
#include <ctype.h>
//  0  1  2  3  4  5  6
//  S1 S2 S3 S4 S5 S6 S7
//  W        W     W  W


#define TRANSLATIONS_END MAX_STORED_TRANSLATIONS_LENGTH - 1


void StrokeStreamInit(StrokeStream *ss)  {
    dictInit(&(ss->d), 0, 150000*10);
    for (int i = 0; i < MAX_STORED_TRANSLATIONS_LENGTH; i++) {
        TranslationInit(ss->translations + i);
    }
    ss->output[0] = '\0';
    ss->outputOld[0] = '\0';
    FormatterInit(&(ss->formatter));
}


void StrokeStreamGetCombinedStrokes(StrokeStream *ss, char* strokes, uint start)  {
    strcpy(strokes, ss->translations[start].stroke);
    for (start++; start < MAX_STORED_TRANSLATIONS_LENGTH; start++)    {
        strcat(strokes, "/");
        strcat(strokes, ss->translations[start].stroke);
    }
}

void StrokeStreamShiftTranslationsDown(StrokeStream *ss)    {
    // shift all the translations down
    for (uint i = 0; i < TRANSLATIONS_END; i++)    {
        TranslationCopy(ss->translations + i, ss->translations + i + 1);
    }
    TranslationInit(ss->translations + TRANSLATIONS_END);
}

// add a stroke to the translation stream and compile the output
// This is the main input function for the translation stream
void StrokeStreamAddStroke(StrokeStream *ss, Stroke stroke)   {

    if (strcmp(stroke, "*") == 0)  {
        StrokeStreamUndo(ss);
        return;
    }
    StrokeStreamShiftTranslationsDown(ss);

    //copy accross the stroke to init it
    strcpy(ss->translations[TRANSLATIONS_END].stroke, stroke);
        
    // compile the output and save it to oldOutput
    // needed to calculate the difference between the strings for typing
    StrokeStreamCompileOutput(ss, ss->outputOld);

    uint index = MAX_STORED_TRANSLATIONS_LENGTH - MAX_NUM_STROKES;
    // start on an translation with a translation
    index = StrokeStreamGetNextTranslation(ss, index);
 
    StrokeStreamSearchForStroke(ss, index);
    // compile with the new stroke added
    absolute_time_t t1 = get_absolute_time();
    StrokeStreamCompileOutput(ss, ss->output);
            absolute_time_t t2 = get_absolute_time();
    volatile int64_t fdsa = absolute_time_diff_us(t1, t2);
}

void StrokeStreamUndo(StrokeStream * ss)
{
    Stroke history[MAX_NUM_STROKES] = {0};
    uint numStrokes = 0;

    StrokeStreamCompileOutput(ss, ss->outputOld);

    // copy the translations into history until you hit a translation with a non zero .english
    while (numStrokes < MAX_NUM_STROKES) {
        if (ss->translations[TRANSLATIONS_END -numStrokes].english[0] != '\0')   {
            strcpy(history[numStrokes], ss->translations[TRANSLATIONS_END -numStrokes].english);
            numStrokes++;
            break;
        }
        strcpy(history[numStrokes], ss->translations[TRANSLATIONS_END -numStrokes].english);
        numStrokes++;
    }
    // shift all the translations up
    for (uint i = 0; i <= TRANSLATIONS_END - numStrokes; i++)    {
        TranslationCopy(ss->translations + i + numStrokes, ss->translations + i );
    }

    // remove the translations at the beginning
    for (uint i = 0; i < numStrokes; i++)    {
        TranslationInit(ss->translations + i);
    }

    // add strokes back in one by one and shift down the translations each time
    while (numStrokes > 1) {
        uint index = MAX_STORED_TRANSLATIONS_LENGTH - MAX_NUM_STROKES;
        index = StrokeStreamGetNextTranslation(ss, index);
        StrokeStreamShiftTranslationsDown(ss);
        strcpy(ss->translations[TRANSLATIONS_END].stroke, history[numStrokes]);
        StrokeStreamSearchForStroke(ss, index);
    }

    // compile the new output with the one fewer stroke
    StrokeStreamCompileOutput(ss, ss->output);
}

void StrokeStreamSearchForStroke(StrokeStream *ss, uint index) {
    //check all the strokes for translation
    char translation[MAX_TRANSLATION_LENGTH];
    char combinedStrokes[NUM_STENO_CHARS * MAX_NUM_STROKES];
    while (index < MAX_STORED_TRANSLATIONS_LENGTH)   {
        if (ss->translations[index].english[0] != '\0' || index == TRANSLATIONS_END)  {

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
    strcpy(ss->translations[TRANSLATIONS_END].english, ss->translations[TRANSLATIONS_END].stroke);
}

uint StrokeStreamGetNextTranslation(StrokeStream *ss, uint index) {
    while (index < TRANSLATIONS_END) {
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
    FormatterDo(&(ss->formatter), ss->translations, output);
}