#include "tusb.h"

#include "typer.h"
#include "time.h"

#include "stroke_stream.h"
#include "keyboard_reader.h"
#include "stroke.h"

#include "regex/regex.h"

enum keyboardState{STENOGRAPHY, QWERTY, ADD_TRANSLATION};

int main() {
    int length;

    StrokeGetter sg;
    strokeGetterInit(&sg);
    StrokeStream a;
    StrokeStreamInit(&a);

    char typingString[MAX_OUTPUT_LENGTH] = {0};

    typerInit();
    readerInit();
    bool stenoMode = true;

    bool keyArray[NUM_KEYS] = {0};
    bool oldKeyArray[NUM_KEYS] = {0};
    enum keyboardState state = STENOGRAPHY;
    
    for (int i = 0; i < 20; i++)    {
        StrokeStreamAddStroke(&a, "HAOEUT");
        
    }
    StrokeStreamAddStroke(&a, "*");
    StrokeStreamAddStroke(&a, "-P");
absolute_time_t t1 = get_absolute_time();
    StrokeStreamAddStroke(&a, "HAOEU");
    StrokeStreamAddStroke(&a, "TET");
    StrokeStreamAddStroke(&a, "RA");
    StrokeStreamAddStroke(&a, "HAOEUT");
    StrokeStreamAddStroke(&a, "KR-P");
    StrokeStreamAddStroke(&a, "*");
    StrokeStreamAddStroke(&a, "STRAO*ET");
            absolute_time_t t2 = get_absolute_time();
    volatile int64_t fdsa = absolute_time_diff_us(t1, t2);
    while (1) {


        tud_task();
        readerGetPressedKeys(keyArray);
        switch(state)   {
            case STENOGRAPHY:
                strokeFromKeys(&sg, keyArray);
                if (sg.stroke[0] != '\0')  {
                    StrokeStreamAddStroke(&a, sg.stroke);
                    getStringDiff(a.outputOld, a.output, typingString);
                    sendString(typingString);
                }
                break;
            
            case QWERTY:
                for (size_t i = 0; i < NUM_KEYS; i++)   {
                    if (keyArray[i] != oldKeyArray[i])  {
                        pressKeys(keyArray);
                        memcpy(oldKeyArray, keyArray, sizeof(keyArray));
                        break;
                    }
                }
                break;
            
            case ADD_TRANSLATION:
                break;
        }
        sleep_us(200);

    }
    //mainn(a.output);

    return 0;
}

/*
#include "pico/stdlib.h"
#include <stdio.h>
#include "bsp/board.h"
#include "stroke_stream.h"
#include "typer.h"


int main() {
    board_led_write(true);
    mainn("asdf");

    StrokeStream a;
    StrokeStreamInit(&a);
    StrokeStreamAddStroke(&a, "UPB");
    
    StrokeStreamAddStroke(&a, "SKEFL");
    
    StrokeStreamAddStroke(&a, "*EDZ");
    StrokeStreamAddStroke(&a, "W*DZ");
    StrokeStreamAddStroke(&a, "HAOEU");
    StrokeStreamAddStroke(&a, "*EDZ");
    StrokeStreamAddStroke(&a, "W*DZ");
    StrokeStreamAddStroke(&a, "-G");
    StrokeStreamAddStroke(&a, "HAOEU");
    StrokeStreamAddStroke(&a, "STRAO*ET");
    StrokeStreamAddStroke(&a, "HAOEU");
    absolute_time_t t1 = get_absolute_time();
    StrokeStreamAddStroke(&a, "HAOEU");
    
    
    //mainn(a.output);
    absolute_time_t t2 = get_absolute_time();
    volatile int64_t fdsa = absolute_time_diff_us(t1, t2);
    volatile int asdf = 0;
    StrokeStreamAddStroke(&a, "HEL");
    StrokeStreamAddStroke(&a, "HRO");
    

    //sendString("asdf");
    //sendString("fdsa");
    //mainnasdf();
    //sendChar('a');
    
}
*/