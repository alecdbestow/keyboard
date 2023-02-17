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



void initDict(Dictionary *d, uint32_t start, uint32_t end);
int readSd(Dictionary *d);
void writeSd(Dictionary *d);
void addTranslation(Dictionary *d, uint8_t *term, uint8_t *translation);
void getTranslation(Dictionary *d, uint8_t *input, uint8_t *output);
uint64_t getPos(Dictionary *d, uint32_t hash);
#endif