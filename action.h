#ifndef _ACTION_H_
#define _ACTION_H_

#include "stroke.h"

#define MAX_TRANSLATION_LENGTH 100

typedef char Output[MAX_TRANSLATION_LENGTH];

typedef struct translation
{
    Stroke stroke;
    char english[MAX_TRANSLATION_LENGTH];
    Output output;
} Translation;

void TranslationOutput(Translation *a);
void TranslationInit(Translation *a);
void TranslationCopy(Translation *dest, const Translation *source);
void TranslationAddEnglish(Translation *a, char *e);
void TranslationRemoveStroke(Translation *a);
void TranslationRemoveTranslation(Translation *a);
#endif