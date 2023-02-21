#include "keyboard_reader.h"
#include "hardware/gpio.h"
#include "tusb.h"

uint8_t inEnumToPin[] = {9, 10, 11, 16, 17, 18, 19, 20, 21, 22, 26, 27};
uint8_t outEnumToPin[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
// the steno keys are listed in order at the beginning


/*
181 180 179 178 175 174 173 172
enum keys{
    U167, U141, U166, U142, U165, U143, U164, U144, U129, U130, U162, U146, 
    U161, U147, U132, U133, U160, U148, U159, U149, U158, U150, U157, U151, 
    U209, U212, U183, U184, U169, U170, U155, U156, U154, U152, U211, U210,
    U198, U197, U195, U196, U181, U182, U168, U140, U126, U127, U200, U199, 
    U193, U194, U179, U180, U128, U202, U201, U191, U192, U177, U178, U163, 
    U145, U131, U204, U203, U189, U190, U175, U176, U206, U205, U187, U188, 
    U173, U174, U134, U135, U208, U207, U185, U186, U171, U172, U136, U137,
    U226, U225, U218, U224, U217, U219, U214, U215, U216, U213, U223, U220,
    U229, U228, U233, U235, U232, U236, U231, U230, U227, U234, U221, U222, NUM_KEYS
};*/
uint8_t codeFromEnum[108];

enum inPin gp0[NUM_IN_PINS] = {U209, U212, U183, U184, U169, U170, U155, U156, U154, U152, U211, U210};
enum inPin gp1[NUM_IN_PINS] = {U198, U197, U195, U196, U181, U182, U167, U168, U141, U140, U126, U127};
enum inPin gp2[NUM_IN_PINS] = {U200, U199, U193, U194, U179, U180, U165, U166, U143, U142, U128, U129};
enum inPin gp3[NUM_IN_PINS] = {U202, U201, U191, U192, U177, U178, U163, U164, U145, U144, U130, U131};
enum inPin gp4[NUM_IN_PINS] = {U204, U203, U189, U190, U175, U176, U161, U162, U147, U146, U132, U133};
enum inPin gp5[NUM_IN_PINS] = {U206, U205, U187, U188, U173, U174, U159, U160, U149, U148, U134, U135};
enum inPin gp6[NUM_IN_PINS] = {U208, U207, U185, U186, U171, U172, U157, U158, U151, U150, U136, U137};
enum inPin gp7[NUM_IN_PINS] = {U226, U225, U218, U224, U217, U219, U214, U215, U216, U213, U223, U220};
enum inPin gp8[NUM_IN_PINS] = {U229, U228, U233, U235, U232, U236, U231, U230, U227, U234, U221, U222};   

enum inPin * allKeys[NUM_OUT_PINS] = {gp0, gp1, gp2, gp3, gp4, gp5, gp6, gp7, gp8};

void initCodes()    {
    codeFromEnum[U167] = HID_KEY_A;
    codeFromEnum[U141] = HID_KEY_Z;  
    codeFromEnum[U166] = HID_KEY_S;  
    codeFromEnum[U142] = HID_KEY_X;  
    codeFromEnum[U165] = HID_KEY_D;  
    codeFromEnum[U143] = HID_KEY_C;  
    codeFromEnum[U164] = HID_KEY_F;  
    codeFromEnum[U144] = HID_KEY_V;  
    codeFromEnum[U129] = HID_KEY_SPACE;  
    codeFromEnum[U130] = HID_KEY_SPACE;  
    codeFromEnum[U163] = HID_KEY_G;  
    codeFromEnum[U145] = HID_KEY_B;  
    codeFromEnum[U162] = HID_KEY_H;  
    codeFromEnum[U146] = HID_KEY_N; 
    codeFromEnum[U132] = HID_KEY_SPACE;  
    codeFromEnum[U133] = HID_KEY_SPACE;  
    codeFromEnum[U161] = HID_KEY_J;  
    codeFromEnum[U147] = HID_KEY_M;  
    codeFromEnum[U160] = HID_KEY_K;  
    codeFromEnum[U148] = HID_KEY_COMMA;  
    codeFromEnum[U159] = HID_KEY_L;  
    codeFromEnum[U149] = HID_KEY_PERIOD;  
    codeFromEnum[U158] = HID_KEY_SEMICOLON;  
    codeFromEnum[U150] = HID_KEY_SLASH;  
    codeFromEnum[U157] = HID_KEY_APOSTROPHE;  
    codeFromEnum[U151] = HID_KEY_SHIFT_LEFT;  
    codeFromEnum[U209] = HID_KEY_BACKSPACE;  
    codeFromEnum[U212] = U184;  
    codeFromEnum[U183] = HID_KEY_BACKSPACE;  
    codeFromEnum[U184] = U170;  
    codeFromEnum[U169] = U155;  
    codeFromEnum[U170] = HID_KEY_BACKSLASH;  
    codeFromEnum[U155] = U154;  
    codeFromEnum[U156] = HID_KEY_ENTER;  
    codeFromEnum[U154] = U211;  
    codeFromEnum[U152] = HID_KEY_SHIFT_RIGHT;  
    codeFromEnum[U211] = U198;  
    codeFromEnum[U210] = U197; 
    codeFromEnum[U198] = U195;  
    codeFromEnum[U197] = U196;  
    codeFromEnum[U195] = HID_KEY_BACKSPACE;  
    codeFromEnum[U196] = U182;  
    codeFromEnum[U181] = HID_KEY_Q;  
    codeFromEnum[U182] = U140;  
    codeFromEnum[U168] = U126;  
    codeFromEnum[U140] = U127;  
    codeFromEnum[U126] = U200;  
    codeFromEnum[U127] = U199;  
    codeFromEnum[U200] = U193;  
    codeFromEnum[U199] = U194;  
    codeFromEnum[U193] = U179;  
    codeFromEnum[U194] = U180;  
    codeFromEnum[U179] = HID_KEY_E;  
    codeFromEnum[U180] = HID_KEY_W;  
    codeFromEnum[U128] = U201;  
    codeFromEnum[U202] = U191;  
    codeFromEnum[U201] = U192;  
    codeFromEnum[U191] = U163;  
    codeFromEnum[U192] = U145;  
    codeFromEnum[U177] = HID_KEY_T;  
    codeFromEnum[U178] = HID_KEY_R;  
    codeFromEnum[U131] = HID_KEY_BACKSPACE;  
    codeFromEnum[U204] = U204;  
    codeFromEnum[U203] = U203;  
    codeFromEnum[U189] = U189;  
    codeFromEnum[U190] = U190;  
    codeFromEnum[U175] = HID_KEY_U;  
    codeFromEnum[U176] = HID_KEY_Y;  
    codeFromEnum[U206] = U206;  
    codeFromEnum[U205] = U205;  
    codeFromEnum[U187] = U187;  
    codeFromEnum[U188] = U188;  
    codeFromEnum[U173] = HID_KEY_O;  
    codeFromEnum[U174] = HID_KEY_I;  
    codeFromEnum[U134] = U134;  
    codeFromEnum[U135] = U135;  
    codeFromEnum[U208] = U208;  
    codeFromEnum[U207] = U207;  
    codeFromEnum[U185] = U185;  
    codeFromEnum[U186] = U186;  
    codeFromEnum[U171] = HID_KEY_BRACKET_LEFT;  
    codeFromEnum[U172] = HID_KEY_P;  
    codeFromEnum[U136] = U136;  
    codeFromEnum[U137] = U137; 
    codeFromEnum[U226] = U226;  
    codeFromEnum[U225] = U225;  
    codeFromEnum[U218] = U218;  
    codeFromEnum[U224] = U224;  
    codeFromEnum[U217] = U217;  
    codeFromEnum[U219] = U219;  
    codeFromEnum[U214] = U214;  
    codeFromEnum[U215] = U215;  
    codeFromEnum[U216] = U216;  
    codeFromEnum[U213] = U213;  
    codeFromEnum[U223] = U223;  
    codeFromEnum[U220] = U220; 
    codeFromEnum[U229] = U229;  
    codeFromEnum[U228] = U228;  
    codeFromEnum[U233] = U233;  
    codeFromEnum[U235] = U235;  
    codeFromEnum[U232] = U232;  
    codeFromEnum[U236] = U236;  
    codeFromEnum[U231] = U231;  
    codeFromEnum[U230] = U230;  
    codeFromEnum[U227] = U227;  
    codeFromEnum[U234] = U234;  
    codeFromEnum[U221] = U221;  
    codeFromEnum[U222] = U222;
}




void readerInit()   {
    initCodes();
    for (size_t i = 0; i < NUM_IN_PINS; i++) {
        gpio_init(inEnumToPin[i]);
        gpio_set_dir(inEnumToPin[i], GPIO_IN);
        gpio_pull_down(inEnumToPin[i]);
    }
    for (size_t i = 0; i < NUM_OUT_PINS; i++) {
        gpio_init(outEnumToPin[i]);
        gpio_set_dir(outEnumToPin[i], GPIO_OUT);
        gpio_put(outEnumToPin[i], 0);
    }
}

void readerGetPressedKeys(bool keyArray[NUM_KEYS]) {
    for (size_t i = 0; i < NUM_OUT_PINS; i++)  {
        
        gpio_put(outEnumToPin[i], 1);
        sleep_us(1);
        for (size_t j = 0; j < NUM_IN_PINS; j++)  {
            if (gpio_get(inEnumToPin[j]))   {
                keyArray[allKeys[i][j]] = 1;
            }
            else    {
                keyArray[allKeys[i][j]] = 0;
            }
        }
        gpio_put(outEnumToPin[i], 0);
        sleep_us(1);
    }
}

uint8_t getCodeFromEnum(uint8_t key)   {
    return codeFromEnum[key];
}
