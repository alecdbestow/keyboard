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
        initAction(a->actions + i);
    }
    for (int i = 0; i < MAX_STORED_ACTIONS_LENGTH -1; i++) {
        a->actions[i].nextAction = a->actions + i + 1;
    }
    a->actions[MAX_STORED_ACTIONS_LENGTH -1].nextAction = a->actions;
    memset(a->output, 0, MAX_OUTPUT_LENGTH);
    memset(a->actionsOutput, 0, MAX_OUTPUT_LENGTH);
    a->end = a->actions;
    //a->end = MAX_STORED_ACTIONS_LENGTH - 1;
    lstrcpy(a->spaceString, " ");
        
    a->ci.outputIndex = a->output;
    a->ci.actionsOutputIndex = a->actionsOutput;

    a->ci.attachNext = false;
    a->ci.capNext = false;
    a->ci.capWord = false;
    a->ci.finishedCapWord = false;
    a->ci.glue = false;
}

//
void ActionStreamGetCombinedStrokes(ActionStream *a, char* strokes, Action* start)  {
    size_t len = lstrcpy(strokes, start->stroke);
    for (Action *i = start->nextAction; i != a->end->nextAction; i = i->nextAction)    {
        len += lstrcpy(strokes+len, "/");
        len += lstrcpy(strokes+len, i->stroke);
    }
}

//adds an a->spacestring to actions output, followed by the string: trans
void addString(ActionStream *a, Action *index, uint8_t *pos, uint8_t *trans) {
    size_t len = strlen(a->actionsOutput);
    index->translation = pos;
    index->length = lstrcpy(index->translation, a->spaceString);
    index->length += lstrcpy(index->translation + index->length, trans);
}


Action *getBounded(ActionStream *a, Action *i) {

    volatile int asdf = i - a->actions;
    while (i - a->actions < 0) {
        i += MAX_STORED_ACTIONS_LENGTH;
    }
    while (i >= a->actions + MAX_STORED_ACTIONS_LENGTH) {
        i -= MAX_STORED_ACTIONS_LENGTH;
    }
    return i;
}


// add a stroke to the action stream and compile the output
// This is the main input function for the action stream
void ActionStreamAddStroke(ActionStream *a, Stroke stroke)   {
    // iterate the end index
    a->end = a->end->nextAction;
    
    for (Action *i = a->end->nextAction; i != a->end; i = i->nextAction)    {
        i->translation -= a->end->length;
    }

    // shift the translations down by one action
    memmove(a->actionsOutput, a->actionsOutput + a->end->length, a->end->length);
    
    // copy the stroke into new stroke into the end index
    memcpy(a->end->stroke, stroke, NUM_STENO_CHARS);

    // compile the output and save it to oldOutput
    // needed to calculate the difference between the strings for typing
    ActionStreamCompileOutput(a);
    strcpy(a->outputOld, a->output);

    char trans[MAX_TRANSLATION_LENGTH];
    char combinedStrokes[NUM_STENO_CHARS * MAX_NUM_STROKES];
            
    Action *index = getBounded(a, a->end - MAX_NUM_STROKES + 1);
    bool foundWord = false;
    //check all the strokes for translation
    absolute_time_t t1 = get_absolute_time();

    uint8_t *pos = a->actionsOutput;
    while (index != a->end) {
        if (index->length > 0) {
            pos = index->translation;
            break;
        }
        index = index->nextAction;
    }

    //start 11 away from the end as the max number of strokes is 11
    while (index != a->end->nextAction)   {
        if (index->length > 0 || index == a->end)  {

            ActionStreamGetCombinedStrokes(a, combinedStrokes, index);
            getTranslation(&(a->d), combinedStrokes, trans);
            if (trans[0] != '\0')   {
                
                addString(a, index, pos, trans);
                
                foundWord = true;
                break;
            }   else    {
                pos += index->length;
            }
        }
        index = index->nextAction;
    }
    absolute_time_t t2 = get_absolute_time();
    volatile int64_t fdsa = absolute_time_diff_us(t1, t2);
    volatile int asdf = 0;

    
    if (!foundWord) {
        addString(a, index, pos, stroke); 
    }   else    {
        for (index = index->nextAction; index != a->end->nextAction; index = index->nextAction)   {
            // remove all the translations following the one created as they have now been encorporated
            index->translation = NULL; 
            index->length = 0;
        }
    }

    // compile with the new stroke added
    ActionStreamCompileOutput(a);

}

// checks wether the actionsOutputIndex is within the translation of the ci.index translation
bool inIndex(ActionStream *a)  {
    return (a->ci.actionsOutputIndex >= a->ci.actionsIndex->translation && 
    a->ci.actionsOutputIndex < a->ci.actionsIndex->translation + a->ci.actionsIndex->length);
}

// outputs into the output stream, a command is seen as a single output
// returns false if the action index is NULL 
// OR actionsOutputIndex is outside of the ci.index action and the checkIndex bool is true
bool outputOnce(ActionStream *a, bool checkIndex)    {
    if (a->ci.actionsOutputIndex && inIndex(a) || !checkIndex)  {
        if (prefix(a->spaceString, a->ci.actionsOutputIndex))   {
            if (a->ci.attachNext)   {
                a->ci.attachNext = false;
                a->ci.actionsOutputIndex += strlen(a->spaceString);
                
            } 
            else if (a->ci.finishedCapWord)  {
                a->ci.capWord = false;
            }

        }
        if (a->ci.actionsOutputIndex[0] == '\\')   {
            a->ci.actionsOutputIndex++;
            a->ci.outputIndex[0] = a->ci.actionsOutputIndex[0];
        }
        else if (a->ci.actionsOutputIndex[0] == '{')   {
            ActionStreamCommandOutput(a);
            return true;
        }
        else if (isalpha(a->ci.actionsOutputIndex[0])) {
            if (a->ci.capNext)    {
                a->ci.outputIndex[0] = toupper(a->ci.actionsOutputIndex[0]);
                a->ci.capNext = false;
            }
            else if (a->ci.lowerNext)   {
                a->ci.outputIndex[0] = tolower(a->ci.actionsOutputIndex[0]);
                a->ci.lowerNext = false;
            }
            else if (a->ci.capWord)   {
                a->ci.outputIndex[0] = toupper(a->ci.actionsOutputIndex[0]);
                a->ci.finishedCapWord = true;
            }
            else    {
                a->ci.outputIndex[0] = a->ci.actionsOutputIndex[0];
            }
        }   else    {
            a->ci.outputIndex[0] = a->ci.actionsOutputIndex[0];
        }
            
        a->ci.actionsOutputIndex++;
        a->ci.outputIndex++;
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
    uint8_t *oldIndex = a->ci.actionsOutputIndex;
    while(inIndex(a) && a->ci.actionsOutputIndex[0] != '}')  {
        for (size_t i = 0; i < NUM_COMMAND_MATCHES; i++)    {
            if (prefix(commands[i].match, a->ci.actionsOutputIndex))  {
                a->ci.actionsOutputIndex = oldIndex;
                return &commands[i];
            }
        }
        a->ci.actionsOutputIndex++;

    }
    a->ci.actionsOutputIndex = oldIndex;
    return NULL;
}

// given a prefix: match, increment the action index over it
void skipPrefix(ActionStream *a, uint8_t *match)    {
    if (prefix(match, a->ci.actionsOutputIndex))    {
        a->ci.actionsOutputIndex += strlen(match);
    }
}

// calls the function in the given compileMatch and passes the appropriate arguments
void callMatchFunc(ActionStream *a, CompileMatch *match, uint8_t order) {
        if (match->arg == CHAR)  {
            match->func(a, match->match[1]);
        }   else  if (match->arg == ORDER)  {
            match->func(a, order);
        }   else  {
            match->func(a, match->arg);
        }
}

// outputs from a '{' char until a '}' and performs the relevant formatting
// actually writes to a->output
void performCommandOutput(ActionStream *a, CompileMatch *match) {
    if (match->prefixPos & PRE && match->prefixPos & POST)    {
        skipPrefix(a, match->match);
    } else if (match->prefixPos & POST) {
        a->ci.actionsOutputIndex++; // skip the first { character
        while (!prefix(match->match, a->ci.actionsOutputIndex))    {
            outputOnce(a, true);
        }
        skipPrefix(a, match->match);
    }
    else if (match->prefixPos & PRE) {
        skipPrefix(a, match->match);
        while (a->ci.actionsOutputIndex[0] != '}' && outputOnce(a, true)) {}
        a->ci.actionsOutputIndex++;
    }
}

// used to call the match function either at the begining of 
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

// updates the index variable and the actionsOutputIndex pointer
bool incrementIndex(ActionStream *a)   {
    a->ci.actionsIndex = a->ci.actionsIndex->nextAction;
    a->ci.actionsOutputIndex = a->ci.actionsIndex->translation;
    if (a->ci.actionsIndex == a->end->nextAction)   {
        return false;
    }   else    {
        return true;
    }
}

void ActionStreamCompileOutput(ActionStream *a)   {
    absolute_time_t t1 = get_absolute_time();

    //set the first index as the end + 2
    a->ci.actionsIndex = a->end->nextAction->nextAction;

    //output from the beginning of each string
    a->ci.actionsOutputIndex = a->actionsOutput;
    a->ci.outputIndex = a->output;
    //while the actionsOutputIndex hasn't finished all actions, output an action, then increment a->ci.actionsOutputIndex[0] != '\0' && 
    do  {
        outputAction(a);
    }    while (incrementIndex(a));
    a->ci.outputIndex[0] = '\0';
    absolute_time_t t2 = get_absolute_time();
    volatile int64_t fdsa = absolute_time_diff_us(t1, t2);
    volatile int asdf = 0;
}