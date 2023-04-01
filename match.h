#ifndef _MATCH_H_
#define _MATCH_H_

#include <stdbool.h>

#define POST 1
#define PRE 1<<1

enum{CAP_FIRST_WORD = 0, LOWER_FIRST_CHAR, UPPER_FIRST_WORD, CHAR, ORDER, ATTACH_LEFT, ATTACH_RIGHT, ATTACH_BOTH};


typedef struct inOut InOut;
typedef struct outputter Outputter;
typedef struct match Match;


typedef struct match
{
    char arg;
    char leftMatchString[10];
    char rightMatchString[10];
    char *start;
    char *end;
} Match;

typedef struct commandMatch  {
    Match match;
} CommandMatch;




bool MatchCheckString(Match *m, char *input);

CommandMatch *CommandMatchFindMatch(char *input);


#endif
