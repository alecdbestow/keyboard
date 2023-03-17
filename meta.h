#ifndef _META_H_
#define _META_H_

#include "match.h"


typedef struct match Match;

InOut metaComma(Outputter *o, Match *m, InOut inOut, char order);
InOut metaStop(Outputter *o, Match *m, InOut inOut, char order);
InOut metaCase(Outputter *o, Match *m, InOut inOut, char order);
InOut metaGlue(Outputter *o, Match *m, InOut inOut, char order);
InOut metaAttach(Outputter *o, Match *m, InOut inOut, char order);
InOut carryCap(Outputter *o, Match *m, InOut inOut, char order);
InOut metaRestart(Outputter *o, Match *m, InOut inOut, char order);


#define MAX_MATCH_LENGTH 10



#endif