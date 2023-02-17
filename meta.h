#ifndef _META_H_
#define _META_H_

#include "action_stream.h"

enum{CAP_FIRST_WORD = 0, LOWER_FIRST_CHAR, UPPER_FIRST_WORD, CHAR, ORDER};

typedef struct action_stream ActionStream;


void metaComma(ActionStream *a, char comma);
void metaStop(ActionStream *a, char stop);
void metaCase(ActionStream *a, char arg);
void metaRetroCase(ActionStream *a, char arg);
void metaGlue(ActionStream *a, char arg);
void metaAttach(ActionStream *a, char arg);
void carryCap(ActionStream *a, char arg);


#define MAX_MATCH_LENGTH 10



#endif