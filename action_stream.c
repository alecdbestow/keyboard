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

static CommandMatch commands[NUM_COMMAND_MATCHES] =    {
    {.match = {.matchString = "{,}", .arg = CHAR}, .func = metaComma},
    {.match = {.matchString = "{:}", .arg = CHAR}, .func = metaComma},
    {.match = {.matchString = "{;}", .arg = CHAR}, .func = metaComma},
    {.match = {.matchString = "{.}", .arg = CHAR}, .func = metaStop},
    {.match = {.matchString = "{!}", .arg = CHAR}, .func = metaStop},
    {.match = {.matchString = "{?}", .arg = CHAR}, .func = metaStop},
    {.match = {.matchString = "{-|}", .arg = CAP_FIRST_WORD}, .func = metaCase},
    {.match = {.matchString = "{>}", .arg = LOWER_FIRST_CHAR}, .func = metaCase},
    {.match = {.matchString = "{<}", .arg = UPPER_FIRST_WORD}, .func = metaCase},
    {.match = {.matchString = "{*-|}", .arg = CAP_FIRST_WORD}, .func = metaRetroCase},
    {.match = {.matchString = "{*>}", .arg = LOWER_FIRST_CHAR}, .func = metaRetroCase},
    {.match = {.matchString = "{*<}", .arg = UPPER_FIRST_WORD}, .func = metaRetroCase},
    {.match = {.matchString = "{^~|", .arg = UPPER_FIRST_WORD}, .func = carryCap},
    {.match = {.matchString = "{~|#}", .arg = UPPER_FIRST_WORD}, .func = carryCap},
    {.match = {.matchString = "{&#}", .arg = ORDER}, .func = metaGlue},
    {.match = {.matchString = "{^}", .arg = CHAR}, .func = metaAttach},
    {.match = {.matchString = "{^#}", .arg = CHAR}, .func = metaAttach},
    {.match = {.matchString = "{#^}", .arg = CHAR}, .func = metaAttach},
    {.match = {.matchString = "{}", .arg = CHAR}, .func = metaRestart}
};



bool regex(Match *match)    {
    uint i = 0;
    while (match->matchString[i] == inputString[i] || (match->matchString[i] == '#' && !()))
    if (match->matchString[0] == '#')  {
        while (!prefix())
            
    }
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
 
    asdf(a, index, stroke);
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

    while (index != a->end->nextAction) {
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
        asdf(a, a->end, history[numStrokes]);
        numStrokes--;
    }
    ActionStreamCompileOutput(a);
    // find last translation
    // for every stroke in that translation but the last one:
    // copy all the old strokes from that translation into an array of strokes
    // remember the old output array
    // delete all the strokes from that translation

    // add them to the action stream one by one
}

void asdf(ActionStream *a, Action *index, Stroke stroke) {
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
    return index;
}

// checks wether the actionsOutputIndex is within the translation of the ci.index translation
bool inIndex(ActionStream *a, char *pos)  {
    return (pos >= a->ci.actionsIndex->translation && 
    pos < a->ci.actionsIndex->translation + a->ci.actionsIndex->length);
}

// outputs into the output stream, a command is seen as a single output
// returns false if the action index is NULL 
// OR actionsOutputIndex is outside of the ci.index action and the checkIndex bool is true
bool outputOnce(ActionStream *a, bool checkIndex)    {
    if (a->ci.actionsOutputIndex && inIndex(a, a->ci.actionsOutputIndex) || !checkIndex)  {
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
        else if (a->ci.actionsOutputIndex[0] == '}')    {
            a->ci.actionsOutputIndex++;
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
void findCommandMatch(ActionStream *a, CommandMatch *cm)    {
    uint numLeft = 0;
    uint numRight = 0;
    
    char *index = a->ci.actionsOutputIndex;
    do   {
        if (index[0] == '\\')   {
            index++;
        }   else if (index[0] == '{')   {
            if (numLeft == 0)   {
                cm->match.start = index;
            }
            numLeft++;
        }   else if (index[0] == '}')   {
            cm->match.end = index;
            numRight++;
        }
        if (numLeft - numRight == 1)    {
            // only search for commands in the top level of the command
            for (size_t i = 0; i < NUM_COMMAND_MATCHES; i++)    {
                if (prefix(commands[i].match.matchString, index))  {
                    cm->func = commands[i].func;
                    strcpy(cm->match.matchString, commands[i].match.matchString);
                    cm->match.arg = commands[i].match.arg;
                }
            }
        }
        index++;

    } while(inIndex(a, index) && numLeft > numRight);
}

// given a prefix: match, increment the action index over it
void skipPrefix(ActionStream *a, uint8_t *match)    {
    if (prefix(match, a->ci.actionsOutputIndex))    {
        a->ci.actionsOutputIndex += strlen(match);
    }
}


// outputs from a '{' char until a '}' and performs the relevant formatting
// actually writes to a->output
void performCommandOutput(ActionStream *a, Match *m) {
    do  {
        skipPrefix(a, m->matchString);
    }   while   (
        outputOnce(a, true) && 
        a->ci.actionsOutputIndex >= m->start && 
        a->ci.actionsOutputIndex <= m->end
    );
}

// used to call the match function either at the begining of 
void ActionStreamCommandOutput(ActionStream *a)    {

    Match match = {0};
    CommandMatch cm;
    findCommandMatch(a, &cm);
    if (cm.match.start && cm.match.end)  {
            cm.func(a, &(cm.match), PRE);
            performCommandOutput(a, &(cm.match));
            cm.func(a, &(cm.match), POST);
    }    else   {
        // TODO
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
}