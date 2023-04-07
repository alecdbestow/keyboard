#include "translation.h"
#include "string.h"

void TranslationOutput(Translation *a)
{
    
}

void TranslationInit(Translation *a)
{
    TranslationRemoveTranslation(a);
    TranslationRemoveStroke(a);
}

void TranslationCopy(Translation *dest, const Translation *source)
{
    strcpy(dest->english, source->english);
    strcpy(dest->stroke, source->stroke);
}

void TranslationAddEnglish(Translation *a, char *e)
{
    strcpy(a->english, e);
}

void TranslationRemoveStroke(Translation *a)  {
    a->stroke[0] = '\0';
}

void TranslationRemoveTranslation(Translation *a)
{
    a->english[0] = '\0';
}
