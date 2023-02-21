#ifndef _STROKE_H_
#define _STROKE_H_

#define NUM_STENO_CHARS 23
#define NUM_STENO_KEYS 34

#include "keyboard_reader.h"


typedef char Stroke[NUM_STENO_CHARS];

typedef struct strokeGetter
{
    bool pressedKeys[NUM_STENO_KEYS];
    Stroke stroke;
    char *keyMap;
    size_t strokeIdx;
} StrokeGetter;

void addChar(StrokeGetter * sg, const char c);

void clearPressedKeys(StrokeGetter *sg);

void strokeGetterInit(StrokeGetter *sg);
bool checkKeys(bool keyArray[NUM_STENO_KEYS], bool keyBoolMap[NUM_STENO_KEYS]);
void addHashKeys(StrokeGetter *sg);
void addDuplicateKeys(StrokeGetter *sg, size_t start, size_t num);
void addKeys(StrokeGetter *sg, size_t start, size_t num);
void addSKeys(StrokeGetter *sg);
void addLeftKeys(StrokeGetter *sg);
void addLeftVowelKeys(StrokeGetter *sg);
void addAsteriskKeys(StrokeGetter *sg);
void addRightVowelKeys(StrokeGetter *sg);
void addRightKeys(StrokeGetter *sg);
void strokeFromKeys(StrokeGetter *sg, bool keyArray[NUM_KEYS]);

#endif