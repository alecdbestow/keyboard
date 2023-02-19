#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "hardware/timer.h"
#include "hardware/structs/usb.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bsp/board.h"
#include "tusb.h"

#include "usb_descriptors.h"
#include "typer.h"
#include "time.h"


int main() {
    initTyper();
    while (1) {
        tud_task();
        if (board_button_read()){
            absolute_time_t t1 = get_absolute_time();
            sendString("asdf");
            absolute_time_t t2 = get_absolute_time();
            volatile int64_t fdsa = absolute_time_diff_us(t1, t2);
            volatile int asdf = 0;
            sleep_ms(100);
        }
    }
    //mainn(a.output);


    return 0;
}

/*
#include "pico/stdlib.h"
#include <stdio.h>
#include "bsp/board.h"
#include "action_stream.h"
#include "typer.h"


int main() {
    board_led_write(true);
    mainn("asdf");

    ActionStream a;
    ActionStreamInit(&a);
    ActionStreamAddStroke(&a, "UPB");
    
    ActionStreamAddStroke(&a, "SKEFL");
    
    ActionStreamAddStroke(&a, "*EDZ");
    ActionStreamAddStroke(&a, "W*DZ");
    ActionStreamAddStroke(&a, "HAOEU");
    ActionStreamAddStroke(&a, "*EDZ");
    ActionStreamAddStroke(&a, "W*DZ");
    ActionStreamAddStroke(&a, "-G");
    ActionStreamAddStroke(&a, "HAOEU");
    ActionStreamAddStroke(&a, "STRAO*ET");
    ActionStreamAddStroke(&a, "HAOEU");
    absolute_time_t t1 = get_absolute_time();
    ActionStreamAddStroke(&a, "HAOEU");
    
    
    //mainn(a.output);
    absolute_time_t t2 = get_absolute_time();
    volatile int64_t fdsa = absolute_time_diff_us(t1, t2);
    volatile int asdf = 0;
    ActionStreamAddStroke(&a, "HEL");
    ActionStreamAddStroke(&a, "HRO");
    

    //sendString("asdf");
    //sendString("fdsa");
    //mainnasdf();
    //sendChar('a');
    
}
*/