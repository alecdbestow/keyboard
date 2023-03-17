#include "action.h"
#include "string.h"

void ActionOutput(Action *a)
{
    
}

void ActionInit(Action *a)
{
    ActionRemoveTranslation(a);
    ActionRemoveStroke(a);
}

void ActionCopy(Action *dest, const Action *source)
{
    strcpy(dest->translation, source->translation);
    strcpy(dest->stroke, source->stroke);
    strcpy(dest->output, source->output);
}

void ActionAddTranslation(Action *a, Translation t)
{
    strcpy(a->translation, t);
}

void ActionRemoveStroke(Action *a)  {
    a->stroke[0] = '\0';
}

void ActionRemoveTranslation(Action *a)
{
    a->translation[0] = '\0';
    a->output[0] = '\0';
}
