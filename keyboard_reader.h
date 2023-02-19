#ifndef _KBD_READER_H_
#define _KBD_READER_H_

#include "pico/stdlib.h"

enum inPin{GP9, GP10, GP11, GP16, GP17, GP18, GP19, GP20, GP21, GP22, GP26, GP27};
enum outPin{GP0, GP1, GP2, GP3, GP4, GP5, GP6, GP7, GP8};

uint8_t gp0[] = {209, 212, 183, 184, 169, 170, 155, 156, 154, 152, 211, 210};
uint8_t gp1[] = {198, 197, 195, 196, 181, 182, 167, 168, 141, 137, 126, 127};
uint8_t gp2[] = {200, 199, 193, 194, 179, 180, 165, 166, 143, 142, 128, 129};
uint8_t gp3[] = {202, 201, 191, 192, 177, 178, 163, 164, 145, 144, 130, 131};
uint8_t gp4[] = {204, 203, 189, 190, 175, 176, 161, 162, 147, 146, 132, 133};
uint8_t gp5[] = {206, 204, 187, 188, 173, 174, 159, 160, 149, 148, 134, 135};
uint8_t gp6[] = {208, 206, 185, 186, 171, 172, 157, 158, 151, 150, 136, 137};
uint8_t gp7[] = {226, 225, 218, 224, 217, 219, 214, 215, 216, 213, 223, 220};
uint8_t gp8[] = {229, 228, 233, 235, 232, 236, 231, 230, 227, 234, 221, 222};   

uint8_t allKeys[gp0, gp1, gp2, gp3, gp4, gp5, gp6, gp7, gp8];

#endif