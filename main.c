#include "tusb.h"

#include "typer.h"
#include "time.h"

#include "action_stream.h"
#include "keyboard_reader.h"
#include "stroke.h"



int main() {
    StrokeGetter sg;
    strokeGetterInit(&sg);

    ActionStream a;
    ActionStreamInit(&a);

    typerInit();

    readerInit();

    ActionStreamAddStroke(&a, "KAP");
    ActionStreamAddStroke(&a, "TAL");

    ActionStreamAddStroke(&a, "KAP");
    ActionStreamAddStroke(&a, "TAL");

    ActionStreamAddStroke(&a, "KAP");
    ActionStreamAddStroke(&a, "TAL");

    ActionStreamAddStroke(&a, "KAP");
    ActionStreamAddStroke(&a, "TAL");

    ActionStreamAddStroke(&a, "KAP");
    ActionStreamAddStroke(&a, "TAL");

    ActionStreamAddStroke(&a, "KAP");
    ActionStreamAddStroke(&a, "TAL");

    ActionStreamAddStroke(&a, "KAP");
    ActionStreamAddStroke(&a, "TAL");
    
    ActionStreamAddStroke(&a, "AL");
    ActionStreamAddStroke(&a, "AL");


/*
    bool stenoMode = true;

    bool keyArray[NUM_KEYS] = {0};
    bool oldKeyArray[NUM_KEYS] = {0};
    char typingString[MAX_OUTPUT_LENGTH] = {0};

    while (1) {
        tud_task();
        readerGetPressedKeys(keyArray);
        if (stenoMode)  {
            strokeFromKeys(&sg, keyArray);
            if (sg.stroke[0] != '\0')  {
                absolute_time_t t1 = get_absolute_time();
                ActionStreamAddStroke(&a, sg.stroke);
                absolute_time_t t2 = get_absolute_time();
                volatile int64_t fdsa = absolute_time_diff_us(t1, t2);
                volatile int asdf = 0;
                getStringDiff(a.outputOld, a.output, typingString);
                sendString(typingString);

            }
        }   else    {
            for (size_t i = 0; i < NUM_KEYS; i++)   {
                if (keyArray[i] != oldKeyArray[i])  {
                    
                    pressKeys(keyArray);

                    memcpy(oldKeyArray, keyArray, sizeof(keyArray));
                    break;
                }

            }
        }

        sleep_us(200);

    }
    //mainn(a.output);

    */
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