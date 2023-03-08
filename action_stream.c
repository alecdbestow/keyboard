#include "action_stream.h"
#include "dictionary.h"
#include "meta.h"
#include <string.h>
#include <ctype.h>
//  0  1  2  3  4  5  6
//  S1 S2 S3 S4 S5 S6 S7
//  W        W     W  W


#define NUM_COMMAND_MATCHES 20
#define IN_INDEX 


// if two matchs apply for a single string, put the more strict match at the top
static CommandMatch commands[NUM_COMMAND_MATCHES] =    {
    {.match = {.leftMatchString = "{,}", .rightMatchString = "", .arg = CHAR}, .func = metaComma},
    {.match = {.leftMatchString = "{:}", .rightMatchString = "", .arg = CHAR}, .func = metaComma},
    {.match = {.leftMatchString = "{;}", .rightMatchString = "", .arg = CHAR}, .func = metaComma},
    {.match = {.leftMatchString = "{.}", .rightMatchString = "", .arg = CHAR}, .func = metaStop},
    {.match = {.leftMatchString = "{!}", .rightMatchString = "", .arg = CHAR}, .func = metaStop},
    {.match = {.leftMatchString = "{?}", .rightMatchString = "", .arg = CHAR}, .func = metaStop},
    {.match = {.leftMatchString = "{-|}", .rightMatchString = "", .arg = CAP_FIRST_WORD}, .func = metaCase},
    {.match = {.leftMatchString = "{>}", .rightMatchString = "", .arg = LOWER_FIRST_CHAR}, .func = metaCase},
    {.match = {.leftMatchString = "{<}", .rightMatchString = "", .arg = UPPER_FIRST_WORD}, .func = metaCase},
    {.match = {.leftMatchString = "{*-|}", .rightMatchString = "", .arg = CAP_FIRST_WORD}, .func = metaRetroCase},
    {.match = {.leftMatchString = "{*>}", .rightMatchString = "", .arg = LOWER_FIRST_CHAR}, .func = metaRetroCase},
    {.match = {.leftMatchString = "{*<}", .rightMatchString = "", .arg = UPPER_FIRST_WORD}, .func = metaRetroCase},
    {.match = {.leftMatchString = "{^~|", .rightMatchString = "}", .arg = UPPER_FIRST_WORD}, .func = carryCap},
    {.match = {.leftMatchString = "{~|", .rightMatchString = "}", .arg = UPPER_FIRST_WORD}, .func = carryCap},
    {.match = {.leftMatchString = "{&", .rightMatchString = "}", .arg = ORDER}, .func = metaGlue},
    {.match = {.leftMatchString = "{^}", .rightMatchString = "", .arg = ATTACH_RIGHT}, .func = metaAttach},
    {.match = {.leftMatchString = "{^", .rightMatchString = "^}", .arg = ATTACH_BOTH}, .func = metaAttach},
    {.match = {.leftMatchString = "{^", .rightMatchString = "}", .arg = ATTACH_LEFT}, .func = metaAttach},
    {.match = {.leftMatchString = "{}", .rightMatchString = "", .arg = CHAR}, .func = metaRestart},
    {.match = {.leftMatchString = "{", .rightMatchString = "^}", .arg = ATTACH_RIGHT}, .func = metaAttach}

};



bool regex(Match *match)    {
}


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
    a->stroke[0] = '\0';
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
        a->actions[i+1].prevAction = a->actions + i;
    }
    a->actions[0].prevAction = a->actions + MAX_STORED_ACTIONS_LENGTH - 1;
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
void addString(ActionStream *a, Action *index, uint8_t *trans) {
    size_t len = strlen(a->actionsOutput);
    if (!index->translation)    {
        index->translation = a->actionsOutput + strlen(a->actionsOutput);
    }
    index->length = lstrcpy(index->translation, a->spaceString);
    index->length += lstrcpy(index->translation + index->length, trans);
}

// add a stroke to the action stream and compile the output
// This is the main input function for the action stream
void ActionStreamAddStroke(ActionStream *a, Stroke stroke)   {
    if (strcmp(stroke, "*") == 0)  {
        ActionStreamUndo(a);
        return;
    }
    // iterate the end index
    a->end = a->end->nextAction;
    
    // shift the translations down by one action
    for (Action *i = a->end->nextAction; i != a->end; i = i->nextAction)    {
        if (i->translation) {
            i->translation -= a->end->length;
        }
    }
    memmove(a->actionsOutput, a->actionsOutput + a->end->length, strlen(a->actionsOutput) + 1 - a->end->length);
    
    // copy the stroke into the end index
    memcpy(a->end->stroke, stroke, NUM_STENO_CHARS);
    a->end->translation = NULL;
    a->end->length = 0;

    // compile the output and save it to oldOutput
    // needed to calculate the difference between the strings for typing
    ActionStreamCompileOutput(a);
    strcpy(a->outputOld, a->output);


    //start MAX_NUM_STROKES away from the end as thats the longest lookup possible
    Action *index = a->end; 
    for (uint i = 0; i < MAX_NUM_STROKES; i++)  {
        index = index->prevAction;
        
    }
    // start on an action with a translation
    index = ActionStreamGetNextTranslation(a, index);
 
    ActionStreamSearchForStroke(a, index, stroke);
    // compile with the new stroke added
    ActionStreamCompileOutput(a);

}

void ActionStreamUndo(ActionStream * a)
{
    Action *index = a->end;

    Stroke history[MAX_NUM_STROKES] = {0};
    uint numStrokes = 0;

    ActionStreamCompileOutput(a);
    strcpy(a->outputOld, a->output);

    while (numStrokes < MAX_NUM_STROKES) {
        a->end = a->end->prevAction;
        strcpy(history[numStrokes], index->stroke);
        index->stroke[0] = '\0';
        index->translation[0] = '\0';
        if (index->length > 0) {
            index->length = 0;
            break;
        }
        numStrokes++;
        index = index->prevAction;
        
    }
    while (numStrokes >= 1) {
        a->end = a->end->nextAction;
        strcpy(a->end->stroke, history[numStrokes]);
        ActionStreamSearchForStroke(a, a->end, history[numStrokes]);
        numStrokes--;
    }
    ActionStreamCompileOutput(a);
}

void ActionStreamSearchForStroke(ActionStream *a, Action *index, Stroke stroke) {
    //check all the strokes for translation
    index = ActionStreamSearchForTranslation(a, index);

    if (index->length == 0) {
        a->end->translation = a->actionsOutput + strlen(a->actionsOutput);
        addString(a, a->end, stroke); 
    }   else    {
        index = index->nextAction;
        ActionStreamWipeTranslations(a, index);
    }
}

Action *ActionStreamGetNextTranslation(ActionStream *a, Action *index) {
    while (index != a->end) {
        if (index->length > 0) {
            break;
        }
        index = index->nextAction;
    }
    return index;
}

void ActionStreamWipeTranslations(ActionStream *a, Action *index) {
    while (index != a->end->nextAction)   {
        // remove all the translations following the one created as they have now been encorporated
        index->translation = NULL; 
        index->length = 0;
        index = index->nextAction;
    }
}

// sets the a->index variable to the location of the last
Action* ActionStreamSearchForTranslation(ActionStream *a, Action *index)  {
    char trans[MAX_TRANSLATION_LENGTH];
    char combinedStrokes[NUM_STENO_CHARS * MAX_NUM_STROKES];
    while (index != a->end->nextAction)   {
        if (index->length > 0 || index == a->end)  {

            ActionStreamGetCombinedStrokes(a, combinedStrokes, index);
            getTranslation(&(a->d), combinedStrokes, trans);
            if (trans[0] != '\0')   {
                
                addString(a, index, trans);
                
                return index;
            }
        }
        index = index->nextAction;
    }
    return a->end;
}

// checks wether the actionsOutputIndex is within the translation of the ci.index translation
bool inIndex(ActionStream *a, char *pos)  {
    return (pos >= a->ci.actionsIndex->translation && 
    pos < a->ci.actionsIndex->translation + a->ci.actionsIndex->length);
}

// outputs into the output stream, a command is seen as a single output
// returns false if the action index is NULL 
// OR actionsOutputIndex is outside of the ci.index action and the checkIndex bool is true
void outputOnce(ActionStream *a)    {
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
        return;
    }
    else if (a->ci.actionsOutputIndex[0] == '}')    {
        a->ci.actionsOutputIndex++;
        return;
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
}

// iterate over an action, and place its output in the action stream output
void outputAction(ActionStream *a) {

    while (inIndex(a, a->ci.actionsOutputIndex))  {
        outputOnce(a);
    }
}

// returns true if a the string: str, starts with the prefix string: pre
bool prefix(const char *pre, const char *str)
{
    return strncmp(pre, str, strlen(pre)) == 0;
}

Func findMatch(ActionStream *a, Match *m)  {
    uint numLeft = 0;
    uint numRight = 0;
    
    char *index = a->ci.actionsOutputIndex;
    do   {
        if (index[0] == '\\')   {
            index++;
        }   else if (index[0] == '{')   {
            if (numLeft == 0)   {
                m->start = index;
            }
            numLeft++;
        }   else if (index[0] == '}')   {
            m->end = index;
            numRight++;
        }
        index++;
    } while(inIndex(a, index) && numLeft > numRight);

    for (size_t i = 0; i < NUM_COMMAND_MATCHES; i++)    {

        bool prefixFound = false;
        bool suffixFound = false;
        index = m->start;
        for (
            uint j = 0; 
            (
                index + j <= m->end + 1 &&
                (index[j] == commands[i].match.leftMatchString[j] ||
                commands[i].match.leftMatchString[j] == '\0')
            );
            j++)   {
                if (commands[i].match.leftMatchString[j] == '\0')   {
                    prefixFound = true;
                    break;
                }
        }
        index = m->end;
        uint len = strlen(commands[i].match.rightMatchString);
        for (
            uint j = 0; 
            (
                prefixFound &&
                index - len + 1 + j > m->start &&
                (index[-len + 1 + j] == commands[i].match.rightMatchString[j] || 
                commands[i].match.rightMatchString[j] == '\0')
            );
            j++)   {
                if (commands[i].match.rightMatchString[j] == '\0')   {
                    suffixFound = true;
                    m->end -= (j - 1);
                    break;
                }
        }
        if (suffixFound && prefixFound) {
            strcpy(m->leftMatchString, commands[i].match.leftMatchString);
            strcpy(m->rightMatchString, commands[i].match.rightMatchString);
            m->arg = commands[i].match.arg;
            return commands[i].func;
        }
    }
    return NULL;
}


// given a prefix: match, increment the action index over it
void skipPrefix(ActionStream *a, char *match)    {
    if (prefix(match, a->ci.actionsOutputIndex))    {
        a->ci.actionsOutputIndex += strlen(match);
    }
}


// outputs from a '{' char until a '}' and performs the relevant formatting
// actually writes to a->output
void performCommandOutput(ActionStream *a, Match *m) {
    skipPrefix(a, m->leftMatchString);
    while   (
        a->ci.actionsOutputIndex < m->end &&
        inIndex(a, a->ci.actionsOutputIndex)
    )   {
        outputOnce(a);
        }
    skipPrefix(a, m->rightMatchString);
}

// used to call the match function either at the begining of 
void ActionStreamCommandOutput(ActionStream *a)    {

    Match match = {0};
    Func func = findMatch(a, &match);
    if (func)  {
            func(a, &match, PRE);
            performCommandOutput(a, &match);
            func(a, &match, POST);
    }    else   {
        // TODO
        a->ci.actionsOutputIndex = match.end + strlen(match.rightMatchString);
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
    a->ci.actionsIndex = a->end->nextAction;

    //output from the beginning of each string
    a->ci.actionsOutputIndex = a->actionsOutput;
    a->ci.outputIndex = a->output;
    //while the actionsOutputIndex hasn't finished all actions, output an action, then increment a->ci.actionsOutputIndex[0] != '\0' && 
    do  {
        outputAction(a);
    }    while (incrementIndex(a));
    a->ci.outputIndex[0] = '\0';
}