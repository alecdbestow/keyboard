#ifndef _DICTIONARY_H_
#define _DICTIONARY_H_

#define BLOCK_SIZE 512
#include "sd_card.h"

typedef struct dictionary
{
    uint32_t start;
    uint32_t end;
    uint32_t pos;
    sd_card_t *pSD;
    uint8_t buf[BLOCK_SIZE];
} Dictionary;



void dictInit(Dictionary *d, uint32_t start, uint32_t end);
int readSd(Dictionary *d);
void writeSd(Dictionary *d);
void addTranslation(Dictionary *d, char *term, char *translation);
void getTranslation(Dictionary *d, char *input, char *output);
uint64_t getPos(Dictionary *d, uint32_t hash);
#endif