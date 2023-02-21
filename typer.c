#include "usb_descriptors.h"
#include "typer.h"
#include "bsp/board.h"


#define KEY_PRESSED_STATE false
#define KEY_RELEASED_STATE true


static char *string;
static uint8_t stringPosition = 0;

void getStringDiff(char *oldOutput, char *output, char *diff)
{
  // skip over the characters that are the same
  size_t i;
  for (i = 0; oldOutput[i] == output[i] && output[i] != '\0'; i++)  {}

  // delete the remaining chars from oldOutput
  size_t j;
  for (j = 0; j < strlen(oldOutput + i); j++)  {
    diff[j] = '\b';
  }

  // add the remaining characters from output
  for (size_t k = 0; k < strlen(output + i); k++) {
    diff[j] = output[i + k];
    j++;
  }
  diff[j] = '\0';
}

void typerInit(void)
{
    board_init();
    tusb_init();
}

void sendChar(char c) {
  uint8_t keycode[6] = { 0 };
  getCode(c, keycode);
  uint8_t modifier = getModifier(c);
  tud_hid_keyboard_report(REPORT_ID_KEYBOARD, modifier, keycode);
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
  

static uint8_t const modifier_map[1] = {
    HID_KEY_SHIFT_LEFT
};


void sendString(char *s) {
  if (s[0] == '\0') {
    return;
  }
  string = s;
  bool pressed = false;
  uint8_t keycodes[6] = {0}; // Map 'A' key
  uint8_t modifier;
  volatile uint i = 0;
  while (1) {
      tud_task();

      if (tud_hid_ready()) {

          if (pressed) {
              
              tud_hid_keyboard_report(0, 0, NULL);
              if (string[0] == '\0')  {
                return;
              }
              pressed = false;
          } else{
              getCode(string[0], keycodes);
              modifier = getModifier(string[0]);
              tud_hid_keyboard_report(0, modifier, keycodes);
              string++;
              pressed = true;
          }
      }
      i++;
  }
  volatile bool fdsa = false;
  return;
}

void pressKeys(bool keyArray[NUM_KEYS]) {
  uint8_t keycodes[6] = {0}; // Map 'A' key
  size_t idx = 0;
  for (size_t i = 0; i < NUM_KEYS; i++) {
    
    if (keyArray[i])  {
      keycodes[idx] = getCodeFromEnum(i);
      if (idx == 6) {
        break;
      } else  {
        idx++;
      }
    }
  }
  while (1) {
    tud_task();
    if (tud_hid_ready()) {
        if (idx == 0) {
            tud_hid_keyboard_report(0, 0, NULL);
        } else{
            tud_hid_keyboard_report(0, 0, keycodes);
        }
        break;
    }
  }
}



void tud_hid_report_complete_cb(uint8_t instance, uint8_t const* report, uint8_t len)
{
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
}