#include "action_stream.hpp"
#include "dictionary.hpp"
#include "meta.hpp"
#include <string.h>
#include <ctype.h>
//  0  1  2  3  4  5  6
//  S1 S2 S3 S4 S5 S6 S7
//  W        W     W  W


#define NUM_COMMAND_MATCHES 18
#define IN_INDEX (true)

static CompileMatch commands[NUM_COMMAND_MATCHES] =    {
    // {.match = "{,}", .func = metaComma, .arg = CHAR, .order = POST, .prefixPos = PRE | POST},
    // {.match = "{:}", .func = metaComma, .arg = CHAR, .order = POST, .prefixPos = PRE | POST},
    // {.match = "{;}", .func = metaComma, .arg = CHAR, .order = POST, .prefixPos = PRE | POST},
    // {.match = "{.}", .func = metaStop, .arg = CHAR, .order = POST, .prefixPos = PRE | POST},
    // {.match = "{!}", .func = metaStop, .arg = CHAR, .order = POST, .prefixPos = PRE | POST},
    // {.match = "{?}", .func = metaStop, .arg = CHAR, .order = POST, .prefixPos = PRE | POST},
    // {.match = "{-|}", .func = metaCase, .arg = CAP_FIRST_WORD, .order = POST, .prefixPos = PRE | POST},
    // {.match = "{>}", .func = metaCase, .arg = LOWER_FIRST_CHAR, .order = POST, .prefixPos = PRE | POST},
    // {.match = "{<}", .func = metaCase, .arg = UPPER_FIRST_WORD, .order = POST, .prefixPos = PRE | POST},
    // {.match = "{*-|}", .func = metaRetroCase, .arg = CAP_FIRST_WORD, .order = PRE, .prefixPos = PRE | POST},
    // {.match = "{*>}", .func = metaRetroCase, .arg = LOWER_FIRST_CHAR, .order = PRE, .prefixPos = PRE | POST},
    // {.match = "{*<}", .func = metaRetroCase, .arg = UPPER_FIRST_WORD, .order = PRE, .prefixPos = PRE | POST},
    // {.match = "{^~|", .func = carryCap, .arg = UPPER_FIRST_WORD, .order = POST, .prefixPos = PRE | POST},
    // {.match = "{~|", .func = carryCap, .arg = UPPER_FIRST_WORD, .order = POST, .prefixPos = PRE | POST},
    // {.match = "{&", .func = metaGlue, .arg = ORDER, .order = PRE | POST, .prefixPos = PRE},
    // {.match = "{^}", .func = metaAttach, .arg = CHAR, .order = POST, .prefixPos = PRE | POST},
    // {.match = "{^", .func = metaAttach, .arg = CHAR, .order = PRE, .prefixPos = PRE},
    // {.match = "^}", .func = metaAttach, .arg = CHAR, .order = POST, .prefixPos = POST}
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

bool prefix(const char *pre, const char *str)
{
    return strncmp(pre, str, strlen(pre)) == 0;
}

Action::Action()  {
    translation = NULL;//memset(a->translation, 0, MAX_TRANSLATION_LENGTH);
    length = 0;
}

CompilerIndex::CompilerIndex(char *actionsOutput, char* output, CircAP idx) : index(idx) {
    actionIndex = actionsOutput;
    outIndex = output;
    capWord = false;
    lowerNext = false;
    capNext = false;
    attachNext = true;
    finishedCapWord = true;
    glue = NULL;
    strcpy(spaceString, " ");
}

bool CompilerIndex::outputOnce(ActionStream &a, bool checkIndex)    {
    if (!IN_INDEX && checkIndex)  {
        return false;
    }
    if (prefix(spaceString, actionIndex))   {
        outputPrefix(); // doesn't output so don't "else if" after
    }
    if (actionIndex[0] == '\\')   {
        outputEscaped();
    } else if(actionIndex[0] == '{') {
        outputCommand(a);
    } else if (isalpha(actionIndex[0])) {
        outputAlpha();
    } else  {
        outputOther();
    }
    return true;
}

void CompilerIndex::outputPrefix()  {
    if (attachNext)   {
        attachNext = false;
        actionIndex += strlen(spaceString);
    } 
    else if (finishedCapWord)  {
        capWord = false;
    }
}

void CompilerIndex::outputEscaped() {
    actionIndex++;
    outIndex[0] = actionIndex[0];
    increment();
}

void CompilerIndex::outputAlpha()   {
    if (capNext)    {
        outIndex[0] = toupper(actionIndex[0]);
        capNext = false;
    }
    else if (lowerNext)   {
        outIndex[0] = tolower(actionIndex[0]);
        lowerNext = false;
    }
    else if (capWord)   {
        outIndex[0] = toupper(actionIndex[0]);
        finishedCapWord = true;
    }
    else    {
        outIndex[0] = actionIndex[0];
    }
    increment();
}

void CompilerIndex::outputOther()   {
    outIndex[0] = actionIndex[0];
    increment();
}

void CompilerIndex::increment() {
    actionIndex++;
    outIndex++;
}

CompileMatch *CompilerIndex::findMatch(const ActionStream &a)  const  {
    char *index = actionIndex;
    while(IN_INDEX && index[0] != '}')  {
        for (size_t i = 0; i < NUM_COMMAND_MATCHES; i++)    {
            if (prefix(commands[i].match, index))  {
                return &commands[i];
            }
        }
        index++;
    }
    return NULL;
}

void CompilerIndex::skipPrefix(const char *prefixString)    {
    if (prefix(prefixString, actionIndex))    {
        actionIndex += strlen(prefixString);
    }
}

void CompilerIndex::callMatchFunc(ActionStream &a, const CompileMatch &match, uint8_t order) {
    if (match.arg == CHAR)  {
        match.func(a, match.match[0]);
    }   else  if (match.arg == ORDER)  {
        match.func(a, order);
    }   else  {
        match.func(a, match.arg);
    }
}

void CompilerIndex::performCommandOutput(ActionStream &a, const CompileMatch &match) {
    if (match.prefixPos & PRE && match.prefixPos & POST)    {
        skipPrefix(match.match);
    } else if (match.prefixPos & POST) {
        actionIndex++; // skip the first { character
        while (!prefix(match.match, actionIndex) && outputOnce(a))    {}
        skipPrefix(match.match);
    }
    else if (match.prefixPos & PRE) {
        skipPrefix(match.match);
        while (actionIndex[0] != '}' && outputOnce(a)) {}
        actionIndex++;
    }
}

void CompilerIndex::outputCommand(ActionStream &a)    {

    CompileMatch *match = findMatch(a);
    bool outputted = false;
    if (match)  {
        if (match->order & PRE) {
            callMatchFunc(a, match[0], PRE);
            performCommandOutput(a, *match);
            outputted = true;
        }
        if (match->order & POST){
            if (!outputted) {
                performCommandOutput(a, *match);
            }
            callMatchFunc(a, match[0], POST);
        }
    }    
}

ActionStream::ActionStream() : endPoint(MAX_OUTPUT_LENGTH), ci(actionsOutput, output, CircAP(MAX_STORED_ACTIONS_LENGTH)) {
    initDict(&dict, 0, 150000*10);
    for (size_t i = 0; i < MAX_STORED_ACTIONS_LENGTH; i++) {
        actions[i] = Action();
    }
    memset(output, 0, MAX_OUTPUT_LENGTH);
    memset(actionsOutput, 0, MAX_OUTPUT_LENGTH);
}



void ActionStream::getCombinedStrokes(char* strokes, CircAP start)  {
    CircAP index = start;
    strokes[0] = '\0';
    while (index != endPoint + 1)    {
        strokes += actions[index].getStroke();
    }
}

// void addString(ActionStream *a, size_t index, char *pos, char *trans) {
//     size_t len = strlen(a->actionsOutput);
//     a->actions[index].translation = pos;
//     a->actions[index].length = lstrcpy(a->actions[index].translation, a->spaceString);
//     a->actions[index].length += lstrcpy(a->actions[index].translation + a->actions[index].length, trans);
// }

size_t ActionStream::addTranslationToOutput(CircAP index, char *pos, bool &found) {
    char trans[MAX_TRANSLATION_LENGTH];
    char combinedStrokes[NUM_STENO_KEYS * MAX_NUM_STROKES];

    getCombinedStrokes(combinedStrokes, index);
    getTranslation(&dict, combinedStrokes, trans);

    if (trans[0] != '\0')   {
        actions[index].setLength(lstrcpy(pos, trans));
        actions[index].setTranslation(pos);
        found = true;
        return 0;

    }   else if (index == endPoint)  {
        actions[index].setLength(lstrcpy(pos, actions[index].getStroke()));
        actions[index].setTranslation(pos);
        found = true;
        return 0;

    }   else    {
        return actions[index].getLength();
    }
}

void ActionStream::updateActionsOutput()    {

    CircAP index = endPoint - MAX_NUM_STROKES;
    char *pos = actionsOutput;
    bool foundWord = false;
    for (; index != endPoint + 1; index++)
    {
        if (foundWord)  {
            actions[index].setLength(0);
            actions[index].setTranslation(NULL);
        }
        else if (actions[index].getStroke() || index == endPoint)  {
            pos += addTranslationToOutput(index, pos, foundWord);
        }
    }
}

void ActionStream::addStroke(const Stroke &stroke)   {
    actions[endPoint].setStroke(stroke);
    endPoint++;
}

void Action::setStroke(const Stroke &s)  {
    stroke.setTranslation(s.getTranslation());
}

void ActionStream::update(const Stroke &stroke) {
    addStroke(stroke);
    updateActionsOutput();
    compileOutput();
}

void ActionStream::outputAction() {
    while (ci.outputOnce(this[0]))  {}
}

void ActionStream::compileOutput()   {
    absolute_time_t t1 = get_absolute_time();

    ci.setIndex(endPoint - MAX_NUM_STROKES);
    ci.setActionIndex(actionsOutput);
    ci.setOutIndex(output);
    while (ci.getIndex() != endPoint + 1)  {
        outputAction();
        ci.addToIndex(1);
    }
    absolute_time_t t2 = get_absolute_time();
    volatile int64_t fdsa = absolute_time_diff_us(t1, t2);
    volatile int asdf = 0;
}