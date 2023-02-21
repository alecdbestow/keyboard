#include "stroke.h"

#define START_HASH_KEYS 0
#define NUM_HASH_KEYS 8
#define START_S_KEYS START_HASH_KEYS + NUM_HASH_KEYS
#define NUM_S_KEYS 2
#define START_LEFT_KEYS START_S_KEYS + NUM_S_KEYS
#define NUM_LEFT_KEYS 6
#define START_LEFT_VOWEL_KEYS START_LEFT_KEYS + NUM_LEFT_KEYS
#define NUM_LEFT_VOWEL_KEYS 2
#define START_ASTERISK_KEYS START_LEFT_VOWEL_KEYS + NUM_LEFT_VOWEL_KEYS
#define NUM_ASTERISK_KEYS 4
#define START_RIGHT_VOWEL_KEYS START_ASTERISK_KEYS + NUM_ASTERISK_KEYS
#define NUM_RIGHT_VOWEL_KEYS 2
#define START_RIGHT_KEYS START_RIGHT_VOWEL_KEYS + NUM_RIGHT_VOWEL_KEYS
#define NUM_RIGHT_KEYS 10


enum inPin strokeOrder[NUM_STENO_KEYS] = {
    U181, U180, U179, U178, U175, 
    U174, U173, U172, U167, U141, 
    U166, U142, U165, U143, U164, 
    U144, U129, U130, U162, U146, 
    U161, U147, U132, U133, U161, 
    U147, U160, U148, U159, U149, 
    U158, U150, U157, U151
};

bool anyKeys[NUM_STENO_KEYS] = {
    true, true, true, true, true, 
    true, true, true, true, true, 
    true, true, true, true, true, 
    true, true, true, true, true, 
    true, true, true, true, true, 
    true, true, true, true, true, 
    true, true, true, true
};

bool hashKeys[NUM_STENO_KEYS] = {
    true, true, true, true, true, 
    true, true, true, false, false, 
    false, false, false, false, false, 
    false, false, false, false, false, 
    false, false, false, false, false, 
    false, false, false, false, false, 
    false, false, false, false, 
};

bool vowelKeys[NUM_STENO_KEYS] = {
    false, false, false, false, false, 
    false, false, false, false, false, 
    false, false, false, false, false, 
    false, true, true, true, true, 
    true, true, true, true, false, 
    false, false, false, false, false, 
    false, false, false, false,  
};

bool upperKeys[NUM_STENO_KEYS] = {
    false, false, false, false, false, 
    false, false, false, true, true, 
    true, false, true, false, true, 
    false, true, true, false, false, 
    false, false, false, false, true, 
    false, true, false, true, false, 
    true, false, false, false,  
};

bool rightKeys[NUM_STENO_KEYS] = {
    false, false, false, false, false, 
    false, false, false, false, false, 
    false, false, false, false, false, 
    false, false, false, false, false, 
    false, false, false, false, true, 
    true, true, true, true, true, 
    true, true, true, true,  
};

char enumToStrokeChar[NUM_STENO_KEYS] = {
    '#', '#', '#', '#', '#', 
    '#', '#', '#', 'S', 'S', 
    'T', 'K', 'P', 'W', 'H', 
    'R', 'A', 'O', '*', '*', 
    '*', '*', 'E', 'U', 'F', 
    'R', 'P', 'B', 'L', 'G', 
    'T', 'S', 'D', 'Z'
};

char enumToStrokeNumberedChar[NUM_STENO_KEYS] = {
    '#', '#', '#', '#', '#', 
    '#', '#', '#', '1', '1', 
    '2', '\0', '3', '\0', '4', 
    '\0', '5', '0', '*', '*', 
    '*', '*', 'E', 'U', '6', 
    '\0', '7', '\0', '8', '\0', 
    '9', '\0', '\0', '\0'
};

void addChar(StrokeGetter *sg, const char c)   {
    sg->stroke[sg->strokeIdx] = c;
    sg->strokeIdx++;
}

void clearPressedKeys(StrokeGetter *sg) {
    for (size_t i = 0; i < NUM_STENO_KEYS; i++)  {
        sg->pressedKeys[i] = false;
    }
}

void strokeGetterInit(StrokeGetter *sg)  {
    clearPressedKeys(sg);
    sg->stroke[0] = '\0';
    sg->strokeIdx = 0;
    
}

bool checkKeys(bool keyArray[NUM_STENO_KEYS], bool keyBoolMap[NUM_STENO_KEYS])    {
    for (size_t i = 0; i < NUM_STENO_KEYS; i++) {
        if (keyBoolMap[i] && keyArray[i])  {
            return true;
        }
    }
    return false;
}

void addHashKeys(StrokeGetter *sg)  {
    if (checkKeys(sg->pressedKeys, hashKeys))  {
        if (checkKeys(sg->pressedKeys, upperKeys)) {
            sg->keyMap = enumToStrokeNumberedChar;
        }   else    {
            addChar(sg, '#');
        }
    }
}

void addDuplicateKeys(StrokeGetter *sg, size_t start, size_t num)    {
    for (size_t i = start; i < start + num; i++) {
        if (sg->pressedKeys[i]) {
            addChar(sg, sg->keyMap[i]);
            return;
        }
    }
}

void addKeys(StrokeGetter *sg, size_t start, size_t num)    {
    for (size_t i = start; i < start + num; i++) {
        if (sg->pressedKeys[i]) {
            addChar(sg, sg->keyMap[i]);
        }
    }
}

void addSKeys(StrokeGetter *sg) {
    addDuplicateKeys(sg, START_S_KEYS, NUM_S_KEYS);
}

void addLeftKeys(StrokeGetter *sg)  {
    addKeys(sg, START_LEFT_KEYS, NUM_LEFT_KEYS);
}

void addLeftVowelKeys(StrokeGetter *sg)  {
    addKeys(sg, START_LEFT_VOWEL_KEYS, NUM_LEFT_VOWEL_KEYS);
}

void addAsteriskKeys(StrokeGetter *sg)  {
    addDuplicateKeys(sg, START_ASTERISK_KEYS, NUM_ASTERISK_KEYS);
}

void addRightVowelKeys(StrokeGetter *sg)  {
    addKeys(sg, START_RIGHT_VOWEL_KEYS, NUM_RIGHT_VOWEL_KEYS);
}

void addRightKeys(StrokeGetter *sg)  {
    addKeys(sg, START_RIGHT_KEYS, NUM_RIGHT_KEYS);
}

void addVowels(StrokeGetter *sg)    {
    addLeftVowelKeys(sg);
    addAsteriskKeys(sg);
    addRightVowelKeys(sg);
}

void strokeFromKeys(StrokeGetter *sg, bool keyArray[NUM_KEYS])   {
    bool empty = true;
    for (size_t i = 0; i < NUM_STENO_KEYS; i++) {
        if (keyArray[strokeOrder[i]])   {
            sg->pressedKeys[i] = true;
            empty = false;
        }
    }
    
    if (empty && checkKeys(sg->pressedKeys, anyKeys))  {
        sg->strokeIdx = 0;
        // Add hash keys
        sg->keyMap = enumToStrokeChar;
        addHashKeys(sg);
        addSKeys(sg);
        addLeftKeys(sg);
        if (checkKeys(sg->pressedKeys, rightKeys))  {
            if (checkKeys(sg->pressedKeys, vowelKeys))  {
                addVowels(sg);
            }   else    {
                addChar(sg, '-');
            }
            addRightKeys(sg);
        }   else    {
            addVowels(sg);
        }
        addChar(sg, '\0');
        clearPressedKeys(sg);

    }   else    {
        sg->stroke[0] = '\0';
    }
    
}