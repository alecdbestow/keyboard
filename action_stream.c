#include "action_stream.h"
#include "string_help.h"
#include <string.h>
#include <ctype.h>
//  0  1  2  3  4  5  6
//  S1 S2 S3 S4 S5 S6 S7
//  W        W     W  W


#define ACTIONS_END MAX_STORED_ACTIONS_LENGTH - 1


void ActionStreamInit(ActionStream *a)  {
    dictInit(&(a->d), 0, 150000*10);
    for (int i = 0; i < MAX_STORED_ACTIONS_LENGTH; i++) {
        ActionInit(a->actions + i);
    }
    a->output[0] = '\0';
    a->outputOld[0] = '\0';
    //a->end = ACTIONS_END;
}

//
void ActionStreamGetCombinedStrokes(ActionStream *a, char* strokes, uint start)  {
    strcpy(strokes, a->actions[start].stroke);
    for (start++; start < MAX_STORED_ACTIONS_LENGTH; start++)    {
        strcat(strokes, "/");
        strcat(strokes, a->actions[start].stroke);
    }
}

// add a stroke to the action stream and compile the output
// This is the main input function for the action stream
void ActionStreamAddStroke(ActionStream *a, Stroke stroke)   {

    for (uint i = 0; i < ACTIONS_END; i++)    {
        ActionCopy(a->actions + i, a->actions + i + 1);
    }
    ActionInit(a->actions + ACTIONS_END);
    strcpy(a->actions[ACTIONS_END].stroke, stroke);
        
    // compile the output and save it to oldOutput
    // needed to calculate the difference between the strings for typing
    ActionStreamCompileOutput(a, a->outputOld);

    uint index = MAX_STORED_ACTIONS_LENGTH - MAX_NUM_STROKES;
    // start on an action with a translation
    index = ActionStreamGetNextTranslation(a, index);
 
    ActionStreamSearchForStroke(a, index);
    // compile with the new stroke added
    ActionStreamCompileOutput(a, a->output);

}
/*
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
*/
void ActionStreamSearchForStroke(ActionStream *a, uint index) {
    //check all the strokes for translation
    char translation[MAX_TRANSLATION_LENGTH];
    char combinedStrokes[NUM_STENO_CHARS * MAX_NUM_STROKES];
    while (index < MAX_STORED_ACTIONS_LENGTH)   {
        if (a->actions[index].translation[0] != '\0' || index == ACTIONS_END)  {

            ActionStreamGetCombinedStrokes(a, combinedStrokes, index);
            getTranslation(&(a->d), combinedStrokes, translation);
            
            if (translation[0] != '\0')   {
                strcpy(a->actions[index].translation, translation);
                ActionStreamWipeTranslations(a, index + 1);
                return;
            }
        }
        index++;
    }
    // if no translations are found, then output the stroke instead
    strcpy(a->actions[ACTIONS_END].translation, a->actions[ACTIONS_END].stroke);
}

uint ActionStreamGetNextTranslation(ActionStream *a, uint index) {
    while (index < ACTIONS_END) {
        if (a->actions[index].translation[0] != '\0') {
            break;
        }
        index++;
    }
    return index;
}

void ActionStreamWipeTranslations(ActionStream *a, uint index) {
    while (index < MAX_STORED_ACTIONS_LENGTH)   {
        // remove all the translations following the one created as they have now been encorporated
        ActionRemoveTranslation(a->actions + index);
        index++;
    }
}

void ActionStreamCompileOutput(ActionStream *a, char *output)   {
    Outputter o;
    o.attachPrev = false;
    OutputterInit(&o);
    uint i = 0;
    output[0] = '\0';

    OutputterCompileAction(&o, a->actions);
    strcat(output, a->actions[i].output);

    for (i += 1; i < MAX_STORED_ACTIONS_LENGTH; i++)  {
        if (a->actions[i].translation[0] != '\0')   {
        OutputterPreCompileAction(&o, a->actions + i);
        InOut inOut = {o.spaceString, output + strlen(output)};
        inOut = OutputterOutputSpace(&o, inOut);
        OutputterOutputNull(&o, inOut);
        OutputterCompileAction(&o, a->actions + i);
        o.attachPrev = false;
        strcat(output, a->actions[i].output);
        }



    }
}