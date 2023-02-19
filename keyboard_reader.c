#include "keyboard_reader.h"
#include "hardware/gpio.h"

void readerInit()   {
    gpio_init_all();
    for (size_t i = 0; i < NUM_IN_PINS) {
        gpio_set_dir(inEnumToNum(i), GPIO_IN);
    }
    for (size_t i = 0; i < NUM_OUT_PINS) {
        gpio_set_dir(outEnumToNum(i), GPIO_OUT);
    }
}

bool[NUM_KEYS] getPressedKeys() {
    for (size_t i = 0; i < NUM_OUT_PINS; i++)  {
        
    }
}