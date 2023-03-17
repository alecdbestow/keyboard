#include "match.h"
#include "meta.h"
#include "string_help.h"
#include "string.h"

#define NUM_COMMAND_MATCHES 17

// if two matchs apply for a single string, put the more strict match at the top
static CommandMatch commands[NUM_COMMAND_MATCHES] =    {
    {.match = {.leftMatchString = "{,}", .rightMatchString = "", .arg = ATTACH_LEFT}, .func = metaComma},
    {.match = {.leftMatchString = "{:}", .rightMatchString = "", .arg = ATTACH_LEFT}, .func = metaComma},
    {.match = {.leftMatchString = "{;}", .rightMatchString = "", .arg = ATTACH_LEFT}, .func = metaComma},
    {.match = {.leftMatchString = "{.}", .rightMatchString = "", .arg = ATTACH_LEFT}, .func = metaStop},
    {.match = {.leftMatchString = "{!}", .rightMatchString = "", .arg = ATTACH_LEFT}, .func = metaStop},
    {.match = {.leftMatchString = "{?}", .rightMatchString = "", .arg = ATTACH_LEFT}, .func = metaStop},
    {.match = {.leftMatchString = "{-|}", .rightMatchString = "", .arg = CAP_FIRST_WORD}, .func = metaCase},
    {.match = {.leftMatchString = "{>}", .rightMatchString = "", .arg = LOWER_FIRST_CHAR}, .func = metaCase},
    {.match = {.leftMatchString = "{<}", .rightMatchString = "", .arg = UPPER_FIRST_WORD}, .func = metaCase},
    {.match = {.leftMatchString = "{^~|", .rightMatchString = "}", .arg = UPPER_FIRST_WORD}, .func = carryCap},
    {.match = {.leftMatchString = "{~|", .rightMatchString = "}", .arg = UPPER_FIRST_WORD}, .func = carryCap},
    {.match = {.leftMatchString = "{&", .rightMatchString = "}", .arg = ATTACH_LEFT}, .func = metaGlue},
    {.match = {.leftMatchString = "{^}", .rightMatchString = "", .arg = ATTACH_RIGHT}, .func = metaAttach},
    {.match = {.leftMatchString = "{^", .rightMatchString = "^}", .arg = ATTACH_BOTH}, .func = metaAttach},
    {.match = {.leftMatchString = "{^", .rightMatchString = "}", .arg = ATTACH_LEFT}, .func = metaAttach},
    {.match = {.leftMatchString = "{}", .rightMatchString = "", .arg = CHAR}, .func = metaRestart},
    {.match = {.leftMatchString = "{", .rightMatchString = "^}", .arg = ATTACH_RIGHT}, .func = metaAttach}
};







bool MatchCheckString(Match *m, char *input)  {
    if (!prefix(m->leftMatchString, input)) {
        return false;
    }   else    {
        m->start = input;
    }
    for (input += strlen(m->leftMatchString); input[0] != '\0' || m->rightMatchString[0] == '\0'; input++)    {
        if (prefix(m->rightMatchString, input)) {
            m->end = input;
            return true;
        }
    }
    return false;
}

CommandMatch *CommandMatchFindMatch(char *input)  {
    for (int i = 0; i < NUM_COMMAND_MATCHES; i++)  {
        if (MatchCheckString(&(commands[i].match), input))    {
            return commands + i;
        }
    }
    return NULL;
}
