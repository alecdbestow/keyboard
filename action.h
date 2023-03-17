#ifndef _ACTION_H_
#define _ACTION_H_

#include "stroke.h"

#define MAX_TRANSLATION_LENGTH 100

typedef char Translation[MAX_TRANSLATION_LENGTH];
typedef char Output[MAX_TRANSLATION_LENGTH];

typedef struct action
{
    Stroke stroke;
    Translation translation;
    Output output;
} Action;

void ActionOutput(Action *a);
void ActionInit(Action *a);
void ActionCopy(Action *dest, const Action *source);
void ActionAddTranslation(Action *a, Translation t);
void ActionRemoveStroke(Action *a);
void ActionRemoveTranslation(Action *a);
#endif