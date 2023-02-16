#include "pico/stdlib.h"
#include <stdio.h>
#include "bsp/board.h"
#include "action_stream.hpp"
#include "typer.hpp"
#include "stroke.hpp"


int main() {
    board_led_write(true);
    ActionStream a;
    absolute_time_t t1 = get_absolute_time();
    char ss1[NUM_STENO_KEYS] = {"asdf"};
    Stroke s1(ss1);
    a.update(s1);
    //mainn(a.output);
    absolute_time_t t2 = get_absolute_time();
    volatile int64_t fdsa = absolute_time_diff_us(t1, t2);
    volatile int asdf = 0;
    

    //sendString("asdf");
    //sendString("fdsa");
    //mainnasdf();
    //sendChar('a');
    
}
