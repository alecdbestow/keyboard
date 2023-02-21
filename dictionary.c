/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

// Example of reading/writing an external serial flash using the PL022 SPI interface

#include <stdio.h>
#include "dictionary.h"
#include "pico/stdlib.h"
#include "FatFs_SPI/sd_driver/sd_card.h"
#include "FatFs_SPI/sd_driver/hw_config.h"

#include <string.h>

#define PREFIX "&&&"
#define PREFIX_LENGTH 3





uint32_t rc_crc32(const char *buf, size_t len)
{
    uint32_t crc = 0;
	static uint32_t table[256];
	static int have_table = 0;
	uint32_t rem;
	uint8_t octet;
	int i, j;
	const char *p, *q;

	/* This check is not thread safe; there is no mutex. */
	if (have_table == 0) {
		/* Calculate CRC table. */
		for (i = 0; i < 256; i++) {
			rem = i;  /* remainder from polynomial division */
			for (j = 0; j < 8; j++) {
				if (rem & 1) {
					rem >>= 1;
					rem ^= 0xedb88320;
				} else
					rem >>= 1;
			}
			table[i] = rem;
		}
		have_table = 1;
	}

	crc = ~crc;
	q = buf + len;
	for (p = buf; p < q; p++) {
		octet = *p;  /* Cast to unsigned octet. */
		crc = (crc >> 8) ^ table[(crc & 0xff) ^ octet];
	}
	return ~crc;
}

uint64_t getPos(Dictionary *d, uint32_t hash) {
    return (hash % (d->end - d->start)) + d->start;
}

void dictInit(Dictionary *d, uint32_t start, uint32_t end) {
    stdio_init_all();
    sd_init_driver();
    d->start = start;
    d->end = end;
    d->pSD = sd_get_by_num(0);
    volatile int asdf = sd_init(d->pSD);
}

void writeSd(Dictionary *d)  {
    
    sd_write_blocks(d->pSD, d->buf, d->pos, 1);
}


int readSd(Dictionary *d) {
    sd_read_blocks(d->pSD, d->buf, d->pos, 1);
    return 0;
}

void addTranslation(Dictionary *d, char *term, char *translation)  {
    getTranslation(d, term, NULL);
    size_t len_term = strlen(term);
    size_t len_trans = strlen(translation);
    memcpy(d->buf, term, len_term);
    memcpy(d->buf+len_term, ": ", 2);
    memcpy(d->buf+len_term+2, translation, len_trans);
    for (int i = len_term+2+len_trans; i < BLOCK_SIZE; i++)    {
        d->buf[i] = 0;
    }
    writeSd(d);

}

void getTranslation(Dictionary *d, char *input, char *output)  {
    size_t len = strlen(input);
    uint32_t hash = rc_crc32(input, len);
    d->pos = getPos(d, hash);
    
    readSd(d);

    while (strncmp(PREFIX, d->buf, PREFIX_LENGTH) == 0)    {
        if (strncmp(input, d->buf+PREFIX_LENGTH, len) == 0)  {
            if (output) {
                strcpy(output, d->buf + PREFIX_LENGTH + len + 2);
            }
            return;
        }   else    {
            char str[20];
            sprintf(str, "%d", hash);
            hash = rc_crc32(str, strlen(input));
            d->pos = getPos(d, hash);
            readSd(d);
        }

    }
    *output = '\0';

}