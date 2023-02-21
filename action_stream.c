#include "action_stream.h"
#include "dictionary.h"
#include "meta.h"
#include <string.h>
#include <ctype.h>
//  0  1  2  3  4  5  6
//  S1 S2 S3 S4 S5 S6 S7
//  W        W     W  W


#define NUM_COMMAND_MATCHES 19
#define IN_INDEX 

static CompileMatch commands[NUM_COMMAND_MATCHES] =    {
    {.match = "{,}", .func = metaComma, .arg = CHAR, .order = POST, .prefixPos = PRE | POST},
    {.match = "{:}", .func = metaComma, .arg = CHAR, .order = POST, .prefixPos = PRE | POST},
    {.match = "{;}", .func = metaComma, .arg = CHAR, .order = POST, .prefixPos = PRE | POST},
    {.match = "{.}", .func = metaStop, .arg = CHAR, .order = POST, .prefixPos = PRE | POST},
    {.match = "{!}", .func = metaStop, .arg = CHAR, .order = POST, .prefixPos = PRE | POST},
    {.match = "{?}", .func = metaStop, .arg = CHAR, .order = POST, .prefixPos = PRE | POST},
    {.match = "{-|}", .func = metaCase, .arg = CAP_FIRST_WORD, .order = POST, .prefixPos = PRE | POST},
    {.match = "{>}", .func = metaCase, .arg = LOWER_FIRST_CHAR, .order = POST, .prefixPos = PRE | POST},
    {.match = "{<}", .func = metaCase, .arg = UPPER_FIRST_WORD, .order = POST, .prefixPos = PRE | POST},
    {.match = "{*-|}", .func = metaRetroCase, .arg = CAP_FIRST_WORD, .order = PRE, .prefixPos = PRE | POST},
    {.match = "{*>}", .func = metaRetroCase, .arg = LOWER_FIRST_CHAR, .order = PRE, .prefixPos = PRE | POST},
    {.match = "{*<}", .func = metaRetroCase, .arg = UPPER_FIRST_WORD, .order = PRE, .prefixPos = PRE | POST},
    {.match = "{^~|", .func = carryCap, .arg = UPPER_FIRST_WORD, .order = POST, .prefixPos = PRE | POST},
    {.match = "{~|", .func = carryCap, .arg = UPPER_FIRST_WORD, .order = POST, .prefixPos = PRE | POST},
    {.match = "{&", .func = metaGlue, .arg = ORDER, .order = PRE | POST, .prefixPos = PRE},
    {.match = "{^}", .func = metaAttach, .arg = CHAR, .order = POST, .prefixPos = PRE | POST},
    {.match = "{^", .func = metaAttach, .arg = CHAR, .order = PRE, .prefixPos = PRE},
    {.match = "^}", .func = metaAttach, .arg = CHAR, .order = POST, .prefixPos = POST},
    {.match = "{}", .func = NULL, .arg = CHAR, .order = POST, .prefixPos = POST}
};


// Returns the length of the string copied
size_t lstrcpy(char *dest, const char *source)    {
    size_t i;
    for (i = 0; source[i] != '\0'; i++) {
        dest[i] = source[i];
    }
    dest[i] = '\0';
    return i;
}

void initAction(Action *a)  {
    memset(a->stroke, 0, NUM_STENO_CHARS);
    a->translation = NULL;//memset(a->translation, 0, MAX_TRANSLATION_LENGTH);
    a->length = 0;
}

void ActionStreamInit(ActionStream *a)  {
    dictInit(&(a->d), 0, 150000*10);
    for (int i = 0; i < MAX_STORED_ACTIONS_LENGTH; i++) {
        initAction(&(a->actions[i]));
    }
    memset(a->output, 0, MAX_OUTPUT_LENGTH);
    memset(a->actionsOutput, 0, MAX_OUTPUT_LENGTH);
    a->end = MAX_STORED_ACTIONS_LENGTH - 1;
    lstrcpy(a->spaceString, " ");
        
    a->ci.outIndex = a->output;
    a->ci.actionIndex = a->actionsOutput;

    a->ci.attachNext = false;
    a->ci.capNext = false;
    a->ci.capWord = false;
    a->ci.finishedCapWord = false;
    a->ci.glue = false;
}

// returns a string which will always been within the max stored actions length
// if an index is incremented over MAX_STORED_ACTIONS_LENGTH it starts from the beginning
// if an index is decremented under 0 then it starts from the end.
size_t getBounded(ActionStream *a, int i) {

    while (i < 0) {
        i += MAX_STORED_ACTIONS_LENGTH;
    }
    if (i >= MAX_STORED_ACTIONS_LENGTH) {
        return (size_t)(i% MAX_STORED_ACTIONS_LENGTH);
    }   else    {
        return (size_t)i;
    }
    
}

//
void ActionStreamGetCombinedStrokes(ActionStream *a, char* strokes, size_t start)  {
    size_t stopPoint = getBounded(a, a->end + 1);
    size_t index = start;
    size_t len = lstrcpy(strokes, a->actions[start].stroke);
    index = getBounded(a, index + 1);
    while (index != stopPoint)    {
        len += lstrcpy(strokes+len, "/");
        len += lstrcpy(strokes+len, a->actions[index].stroke);
        index = getBounded(a, index + 1);
    }
}

//adds an a->spacestring to actions output, followed by the string: trans
void addString(ActionStream *a, size_t index, uint8_t *pos, uint8_t *trans) {
    size_t len = strlen(a->actionsOutput);
    a->actions[index].translation = pos;
    a->actions[index].length = lstrcpy(a->actions[index].translation, a->spaceString);
    a->actions[index].length += lstrcpy(a->actions[index].translation + a->actions[index].length, trans);
}

// add a stroke to the action stream and compile the output
// This is the main input function for the action stream
void ActionStreamAddStroke(ActionStream *a, Stroke stroke)   {
    a->end = getBounded(a, a->end + 1);
    memmove(a->actionsOutput, a->actionsOutput + a->actions[a->end].length, a->actions[a->end].length);
    initAction(a->actions + a->end);
    memcpy(a->actions[a->end].stroke, stroke, NUM_STENO_CHARS);

    ActionStreamCompileOutput(a);

    strcpy(a->outputOld, a->output);
    char trans[MAX_TRANSLATION_LENGTH];
    char combinedStrokes[NUM_STENO_CHARS * MAX_NUM_STROKES];
            
    size_t index;
    bool foundWord = false;
    //check all the strokes for translation
    absolute_time_t t1 = get_absolute_time();
    uint8_t *pos = a->actionsOutput;
    //start 11 away from the end as the max number of strokes is 11
    for (index = getBounded(a, a->end - MAX_NUM_STROKES + 1); index != a->end; index = getBounded(a, index + 1))   {
        if (a->actions[index].translation)  {

            ActionStreamGetCombinedStrokes(a, combinedStrokes, index);
            getTranslation(&(a->d), combinedStrokes, trans);
            if (trans[0] != '\0')   {
                
                addString(a, index, pos, trans);
                index = getBounded(a, index + 1);
                foundWord = true;
                break;
            }   else    {
                pos += a->actions[index].length;
            }
        }
    }
    absolute_time_t t2 = get_absolute_time();
    volatile int64_t fdsa = absolute_time_diff_us(t1, t2);
    volatile int asdf = 0;
    if (!foundWord && index == a->end) {
        getTranslation(&(a->d), stroke, trans);
        if (trans[0] != '\0')   {
            addString(a, index, a->actionsOutput + strlen(a->actionsOutput), trans);
        }   else    {
            addString(a, index, a->actionsOutput + strlen(a->actionsOutput), stroke); // output the stroke if there are no translations found
        }
    }   else    {
        for (; index != getBounded(a, a->end + 1); index = getBounded(a, index + 1))   {
            a->actions[index].translation = NULL; // remove all the translations following the one created as they have now been encorporated
        }
    }
    // if no translation found

    if (a->ci.actionIndex - a->output > MAX_OUTPUT_LENGTH - MAX_TRANSLATION_LENGTH) {

    }
    ActionStreamCompileOutput(a);

}

bool inIndex(ActionStream *a)  {
    return (a->ci.actionIndex >= a->actions[a->ci.index].translation && 
    a->ci.actionIndex < a->actions[a->ci.index].translation + a->actions[a->ci.index].length);
}

// outputs into the output stream, a command is seen as a single output
bool outputOnce(ActionStream *a, bool checkIndex)    {
    if (a->ci.actionIndex && inIndex(a) || !checkIndex)  {
        if (prefix(a->spaceString, a->ci.actionIndex))   {
            if (a->ci.attachNext)   {
                a->ci.attachNext = false;
                a->ci.actionIndex += strlen(a->spaceString);
                
            } 
            else if (a->ci.finishedCapWord)  {
                a->ci.capWord = false;
            }

        }
        if (a->ci.actionIndex[0] == '\\')   {
            a->ci.actionIndex++;
            a->ci.outIndex[0] = a->ci.actionIndex[0];
        }
        else if (a->ci.actionIndex[0] == '{')   {
            ActionStreamCommandOutput(a);
            return true;
        }
        else if (isalpha(a->ci.actionIndex[0])) {
            if (a->ci.capNext)    {
                a->ci.outIndex[0] = toupper(a->ci.actionIndex[0]);
                a->ci.capNext = false;
            }
            else if (a->ci.lowerNext)   {
                a->ci.outIndex[0] = tolower(a->ci.actionIndex[0]);
                a->ci.lowerNext = false;
            }
            else if (a->ci.capWord)   {
                a->ci.outIndex[0] = toupper(a->ci.actionIndex[0]);
                a->ci.finishedCapWord = true;
            }
            else    {
                a->ci.outIndex[0] = a->ci.actionIndex[0];
            }
        }   else    {
            a->ci.outIndex[0] = a->ci.actionIndex[0];
        }
            
        a->ci.actionIndex++;
        a->ci.outIndex++;
        return true;
    }   else    {
        return false;
    }  
}

// iterate over an action, and place its output in the action stream output
void outputAction(ActionStream *a) {

    while (outputOnce(a, true))  {}
}

// returns true if a the string: str, starts with the prefix string: pre
bool prefix(const char *pre, const char *str)
{
    return strncmp(pre, str, strlen(pre)) == 0;
}


// iterate over all the possible command matches and return the first match
CompileMatch *findMatch(ActionStream *a)    {
    uint8_t *oldIndex = a->ci.actionIndex;
    while(inIndex(a) && a->ci.actionIndex[0] != '}')  {
        for (size_t i = 0; i < NUM_COMMAND_MATCHES; i++)    {
            if (prefix(commands[i].match, a->ci.actionIndex))  {
                a->ci.actionIndex = oldIndex;
                return &commands[i];
            }
        }
        a->ci.actionIndex++;

    }
    a->ci.actionIndex = oldIndex;
    return NULL;
}

// given a prefix: match, increment the action index over it
void skipPrefix(ActionStream *a, uint8_t *match)    {
    if (prefix(match, a->ci.actionIndex))    {
        a->ci.actionIndex += strlen(match);
    }
}

void callMatchFunc(ActionStream *a, CompileMatch *match, uint8_t order) {
        if (match->arg == CHAR)  {
            match->func(a, match->match[0]);
        }   else  if (match->arg == ORDER)  {
            match->func(a, order);
        }   else  {
            match->func(a, match->arg);
        }
}

void performCommandOutput(ActionStream *a, CompileMatch *match) {
    if (match->prefixPos & PRE && match->prefixPos & POST)    {
        skipPrefix(a, match->match);
    } else if (match->prefixPos & POST) {
        a->ci.actionIndex++; // skip the first { character
        while (!prefix(match->match, a->ci.actionIndex))    {
            outputOnce(a, true);
        }
        skipPrefix(a, match->match);
    }
    else if (match->prefixPos & PRE) {
        skipPrefix(a, match->match);
        while (a->ci.actionIndex[0] != '}' && outputOnce(a, true)) {}
        a->ci.actionIndex++;
    }
}

void ActionStreamCommandOutput(ActionStream *a)    {

    CompileMatch *match = findMatch(a);
    bool outputted = false;
    if (match)  {
        if (match->order & PRE) {
            callMatchFunc(a, match, PRE);
            performCommandOutput(a, match);
            outputted = true;
        }
        if (match->order & POST){
            if (!outputted) {
                performCommandOutput(a, match);
            }
            callMatchFunc(a, match, POST);
        }
    }    
}

bool incrementIndex(ActionStream *a)   {
    a->ci.index = getBounded(a, a->ci.index + 1);
    a->ci.actionIndex = a->actions[a->ci.index].translation;
    if (a->ci.index == getBounded(a, a->end + 1))   {
        return false;
    }   else    {
        return true;
    }
}

void ActionStreamCompileOutput(ActionStream *a)   {
    absolute_time_t t1 = get_absolute_time();

    //set the first index as the end + 2
    a->ci.index = getBounded(a, a->end + 2);

    //output from the beginning of each string
    a->ci.actionIndex = a->actionsOutput;
    a->ci.outIndex = a->output;
    //while the actionIndex hasn't finished all actions, output an action, then increment a->ci.actionIndex[0] != '\0' && 
    do  {
        outputAction(a);
    }    while (incrementIndex(a));
    a->ci.outIndex[0] = '\0';
    absolute_time_t t2 = get_absolute_time();
    volatile int64_t fdsa = absolute_time_diff_us(t1, t2);
    volatile int asdf = 0;
}