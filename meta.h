#ifndef _META_H_
#define _META_H_

#include "action_stream.h"

enum{CAP_FIRST_WORD = 0, LOWER_FIRST_CHAR, UPPER_FIRST_WORD, CHAR, ORDER, ATTACH_LEFT, ATTACH_RIGHT, ATTACH_BOTH};

typedef struct action_stream ActionStream;
typedef struct match Match;

void metaComma(ActionStream *a, Match *m, char order);
void metaStop(ActionStream *a, Match *m, char order);
void metaCase(ActionStream *a, Match *m, char order);
void metaRetroCase(ActionStream *a, Match *m, char order);
void metaGlue(ActionStream *a, Match *m, char order);
void metaAttach(ActionStream *a, Match *m, char order);
void carryCap(ActionStream *a, Match *m, char order);
void metaRestart(ActionStream *a, Match *m, char order);


#define MAX_MATCH_LENGTH 10



#endif