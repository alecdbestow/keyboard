/* 
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bsp/board.h"
#include "tusb.h"

#include "usb_descriptors.h"

#include "typer.h"



static bool has_keyboard_key = false;
static char *string = "abcdefghij\0";

void initTyper(void) {
  board_init();
  tusb_init();
}

void getCode(char c, uint8_t keycode[6])  {
  uint8_t const conv_table[128][2] =  { HID_ASCII_TO_KEYCODE };

  keycode[0] = conv_table[c][1];
}

uint8_t getModifier(char c) {
  uint8_t const conv_table[128][2] =  { HID_ASCII_TO_KEYCODE };
  uint8_t modifier = 0;
  if ( conv_table[c][0] ) modifier = KEYBOARD_MODIFIER_LEFTSHIFT;
  return modifier;
}

void sendString(char * s) {
  string = s;

  while(*string != '\0' || has_keyboard_key)  {
    tud_task();
    hid_task();
  }
}


//--------------------------------------------------------------------+
// USB HID
//--------------------------------------------------------------------+

static bool send_hid_report(uint8_t report_id, uint32_t btn)
{
  // skip if hid is not ready yet
  if ( !tud_hid_ready() ) return false;

  // use to avoid send multiple consecutive zero report for keyboard
  

  if ( !has_keyboard_key )
  {

    uint8_t keycode[6] = { 0 };
    getCode(*string, keycode);
    tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, keycode);
    string++;
    has_keyboard_key = true;
  }else
  {
    // send empty key report if previously has key pressed
    if (has_keyboard_key) tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, NULL);
    has_keyboard_key = false;
  }
    
  return true;
}

// Every 10ms, we will sent 1 report for each HID profile (keyboard, mouse etc ..)
// tud_hid_report_complete_cb() is used to send the next report after previous one is complete
void hid_task(void)
{
  // Poll every 10ms
  if ( tud_suspended())
  {
    // Wake up host if we are in suspend mode
    // and REMOTE_WAKEUP feature is enabled by host
    tud_remote_wakeup();
  }else
  {
    // Send the 1st of report chain, the rest will be sent by tud_hid_report_complete_cb()
    send_hid_report(REPORT_ID_KEYBOARD, 1);
  }
  
  // Remote wakeup

}



/*
// Invoked when sent REPORT successfully to host
// Application can use this to send the next report
// Note: For composite reports, report[0] is report ID
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const* report, uint8_t len)
{
  static bool hasKeyboardKey = false;
  if (hasKeyboardKey) {
    stringPosition++;
    if (string[stringPosition] == '\0') {
      return;
    } else  {
      sendChar(string[stringPosition]);
    }
    hasKeyboardKey = true;

    
  } else  {
    tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, NULL);
    hasKeyboardKey = true;
  }

  
}
*/
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
  (void) instance;
  (void) report_id;
  (void) report_type;
  (void) buffer;
  (void) reqlen;
  return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
(void) instance;
}

void tud_hid_report_complete_cb(uint8_t instance, uint8_t const* report, uint8_t len)
{
  (void) instance;
  (void) len;
  send_hid_report(REPORT_ID_KEYBOARD, board_button_read());
}
