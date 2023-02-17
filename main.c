#include "pico/stdlib.h"
#include <stdio.h>
#include "bsp/board.h"
#include "action_stream.h"
#include "typer.h"


int main() {
    board_led_write(true);
    ActionStream a;
    initActionStream(&a);
    addStroke(&a, "UPB");
    
    addStroke(&a, "SKEFL");
    
    addStroke(&a, "*EDZ");
    addStroke(&a, "W*DZ");
    addStroke(&a, "HAOEU");
    addStroke(&a, "*EDZ");
    addStroke(&a, "W*DZ");
    addStroke(&a, "-G");
    addStroke(&a, "HAOEU");
    addStroke(&a, "STRAO*ET");
    addStroke(&a, "HAOEU");
    absolute_time_t t1 = get_absolute_time();
    addStroke(&a, "HAOEU");
    
    
    //mainn(a.output);
    absolute_time_t t2 = get_absolute_time();
    volatile int64_t fdsa = absolute_time_diff_us(t1, t2);
    volatile int asdf = 0;
    addStroke(&a, "HEL");
    addStroke(&a, "HRO");
    

    //sendString("asdf");
    //sendString("fdsa");
    //mainnasdf();
    //sendChar('a');
    
}
