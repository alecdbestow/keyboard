#ifndef _HID_H_
#define _HID_H_

#include "tusb.h"
#include "keyboard_reader.h"
// call before sending any strings
void initTyper(void);

// Send all of the contents in the string over usb
void sendString(char *);

// get whether the shift key is pressed (i think?)
uint8_t getModifier(char c);

// get the keycode for the given char
void getCode(char c, uint8_t keycode[6]);

// send a single char over usb
void sendChar(char c);

// press keys
void pressKeys(bool keyArray[NUM_KEYS]);

#endif