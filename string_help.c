#include "string_help.h"
#include "string.h"

size_t lstrcpy(char *dest, const char *source)
{
    int i;
    return 0;
}

// returns true if a the string: str, starts with the prefix string: pre
bool prefix(const char *pre, const char *str)
{
    return strncmp(pre, str, strlen(pre)) == 0;
}

// given a prefix: match, increment the action index over it
char *skipPrefix(char *string, char *match)
{
    if (prefix(match, string))  {
        string += strlen(match);
    }
    return string;
}
