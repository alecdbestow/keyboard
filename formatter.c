#include "formatter.h"
#include "meta.h"
#include "string.h"
#include <ctype.h>

void FormatterInit(Formatter *f)    {
    f->atom = compileMatchingRegex("(\\\\\\{|\\\\\\}|[^\\{\\}])+|\\{(\\\\\\{|\\\\\\}|[^\\{\\}])*\\}");
    f->word = compileMatchingRegex("(\\d+([.,]\\d+)+|[\'\\w]+[-\\w\']*|[^\\w\\s]+)\\s*");
    
    MetasInit();
    Translation t;
    for (int i = 0; i < MAX_NUM_WORDS; i++) {
        WordInit(f->words + i);
    }
    //add an action to be used as a space string
    //it can be any single action command
    strcpy(t.english, " ");
    ActionsUpdateTranslation(f, &(f->spaceAction), &t);
    FormatterFormatActions(&(f->spaceAction), 1);
}

int ActionsUpdateTranslation(Formatter *f, Action *actions, Translation *translation)  {
    int length, exitNum;
    int i = 0;
    char *start = translation->english;
    while (startsWithRegexN(f->atom, start, MAX_TRANSLATION_LENGTH, &length, &exitNum)) {
        ActionInit(actions + i);
        memcpy(actions[i].text, start, length*sizeof(char));
        actions[i].text[length] = '\0';
        actions[i].length = length;
        start += length;
        i++;
    }
    return i;
}

int FormatterUpdateActionsTranslations(Formatter *f, Translation *translations, int numTranslations)   {
    int i = 0;
    // split all the translations into a long list of actions
    for (int j = 0; j < numTranslations; j++) {
        i += ActionsUpdateTranslation(f, f->actions + i, translations + j);
    }
    return i;
}

void WordInit(Word *w)
{
    w->length = 0;
    w->word = NULL;
}

void FormatterFormatActions(Action *actions, int numActions)    {
    ActionHandler ah;
    ActionHandlerInit(&ah, actions, numActions);
    for (int j = 0; j < numActions; j++)   {
        ActionHandlerSet(&ah, actions + j);
        ActionFormat(actions + j, &ah);
    }
}

void FormatterOutput(Formatter *f, int numActions, char *output)    {
    char *outputPos = output;
    
    for (int i = 0; i < numActions; i++)    {
        char *actionOutputPos = f->totalActions[i].action->output;
        f->totalActions[i].pos = outputPos;
        int length = strlen(actionOutputPos);
        strcpy(outputPos, actionOutputPos);
        outputPos += length;
    }
}

void capFirstChar(Word *w)  {
    w->word[0] = toupper(w->word[0]);
}

void upperWord(Word *w) {
    for (int i = 0; i < w->length; i++) {
        w->word[i] = toupper(w->word[i]);
    }
}

void lowerFirstChar(Word *w)    {
    w->word[0] = tolower(w->word[0]);
}

void FormatterApplyCase(Formatter *f, int numActionPos) {
    for (int j = 0; j < numActionPos; j++) {
        if (f->totalActions[j].action->capFirstChar)    {
            capFirstChar(f->totalActions[j].wordPtr);
        }
        if (f->totalActions[j].action->upperFirstWord)    {
            upperWord(f->totalActions[j].wordPtr);
        }
        if (f->totalActions[j].action->lowerFirstChar)  {
            lowerFirstChar(f->totalActions[j].wordPtr);
        }
        if (f->totalActions[j].wordPtr > f->words) {
            if (f->totalActions[j].action->capPrevWordFirstChar)    {
                capFirstChar(f->totalActions[j].wordPtr - 1);
            }
            if (f->totalActions[j].action->upperPrevWord)    {
                upperWord(f->totalActions[j].wordPtr - 1);
            }
            if (f->totalActions[j].action->lowerPrevWordFirstChar)  {
                lowerFirstChar(f->totalActions[j].wordPtr - 1);
            }
        }
    }
}

// Add spaces between actions where needed
// return the total number of actions including spaces
int FormatterAddSpaces(Formatter *f, int numActions)    {
    int i = 0;
        // add the space action between other actions
    for (int j = 0; j < numActions; j++)   {
        if (!f->actions[j].prevAttach && (j == 0 || (!f->actions[j-1].nextAttach && f->actions[j-1].output[0] != '\0')))   {
            f->totalActions[i].action = &(f->spaceAction);
            i++;
        }
        f->totalActions[i].action = f->actions + j;
        i++;
    }
    return i;
}

int FormatterFindWords(Formatter *f, char *output)   {
    int length;
    char *wordPos = output;
    ActionPos *ap = f->totalActions;
    //have a pointer from each action to the beginning of the word they are a part of
    int i = 0;
    while(wordPos[0] != '\0')    {
        if (startsWithRegexN(f->word, wordPos, MAX_TRANSLATION_LENGTH, &(f->words[i].length), NULL))  {
            // while the sart position of all the actions is within the the word, make the action point to that word
            f->words[i].word = wordPos;
            wordPos += f->words[i].length;
            i++;
        }   else    {
            wordPos++;
        }
    }
    return i;
}

void ActionPosConnectWords(ActionPos *aps, Word *words, int numActions, int numWords) {
    int i = 0, j = 0;
    while (i < numWords)    {
        while (j < numActions && aps[j].pos < words[i].word + words[i].length)  {
            aps[j].wordPtr = words+i;
            j++;
        }
        i++;
    }
}


void FormatterDo(Formatter *f, Translation *translations, char *output)
{

    int numActions = FormatterUpdateActionsTranslations(f, translations, MAX_STORED_TRANSLATIONS_LENGTH);
    // format all of the actions 4in the list
    FormatterFormatActions(f->actions, numActions);
    f->actions[0].prevAttach = true;
    numActions = FormatterAddSpaces(f, numActions);
    FormatterOutput(f, numActions, output);
    int numWords = FormatterFindWords(f, output);
    ActionPosConnectWords(f->totalActions, f->words, numActions, numWords);
    FormatterApplyCase(f, numActions);
}

void ActionFormatCommand(Action *a, ActionHandler *ah)
{
    Func metaFunction = MetasGet(a);
    if (metaFunction)   {
        metaFunction(a, ah);
    }
}

void ActionFormatWords(Action *a)
{
    strcpy(a->output, a->text);
}

void ActionInit(Action *a)
{
    a->length = 0;

    a->prevAttach = false;
    a->nextAttach = false;

    a->capFirstChar = false;
    a->lowerFirstChar = false;
    a->upperFirstWord = false;

    a->capPrevWordFirstChar = false;
    a->lowerPrevWordFirstChar = false;
    a->upperPrevWord = false;

    a->glue = false;
    a->orthography = false;
    a->output[0] = '\0';
}

void ActionFormat(Action *a, ActionHandler *ah)
{
    if (a->text[0] == '{')  {
        ActionFormatCommand(a, ah);
    }   else    {
        ActionFormatWords(a);
    }
}

void ActionHandlerSet(ActionHandler *ah, Action *a)
{
    if (a)  {
        ah->action = a;
    }
}

Action *ActionHandlerNextAction(ActionHandler *ah)
{
    if (ah->action + 1 < ah->beginning + ah->numActions)  {
        return ah->action + 1;
    }   else    {
        return NULL;
    }
    
}

Action *ActionHandlerPrevAction(ActionHandler *ah)
{
    if (ah->action -1 >= ah->beginning && 
    ah[1].action->text[0] != '\0')  {
        return ah->action - 1;
    }   else    {
        return NULL;
    }
}

void ActionHandlerInit(ActionHandler *ah, Action *beginning, int numActions)    {
    ah->numActions = numActions;
    ah->beginning = beginning;
    ah->action = beginning;
}