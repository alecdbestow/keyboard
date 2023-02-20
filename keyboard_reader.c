#include "keyboard_reader.h"
#include "hardware/gpio.h"
#include "tusb.h"

uint8_t inEnumToPin[] = {9, 10, 11, 16, 17, 18, 19, 20, 21, 22, 26, 27};
uint8_t outEnumToPin[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
// the steno keys are listed in order at the beginning


/*
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
uint8_t codeFromEnum[] = {
    HID_KEY_A, HID_KEY_Z, HID_KEY_S, HID_KEY_X, HID_KEY_D, HID_KEY_C, HID_KEY_F, HID_KEY_V, HID_KEY_SPACE, HID_KEY_SPACE, HID_KEY_G, HID_KEY_B, 
    HID_KEY_H, HID_KEY_N, HID_KEY_SPACE, HID_KEY_SPACE, HID_KEY_J, HID_KEY_M, HID_KEY_K, HID_KEY_COMMA, HID_KEY_L, HID_KEY_PERIOD, HID_KEY_SEMICOLON, HID_KEY_SLASH, 
    HID_KEY_APOSTROPHE, HID_KEY_SHIFT_LEFT, HID_KEY_BACKSPACE, U184, HID_KEY_BACKSPACE, U170, U155, HID_KEY_BACKSLASH, U154, HID_KEY_ENTER, U211, HID_KEY_SHIFT_RIGHT,
    U198, U197, U195, U196, HID_KEY_BACKSPACE, U182, HID_KEY_Q, U140, U126, U127, U200, U199, 
    U193, U194, U179, U180, HID_KEY_E, HID_KEY_W, U201, U191, U192, U163, U145, HID_KEY_T, 
    HID_KEY_R, HID_KEY_BACKSPACE, U204, U203, U189, U190, HID_KEY_U, HID_KEY_Y, U206, U205, U187, U188, 
    HID_KEY_O, HID_KEY_I, U134, U135, U208, U207, U185, U186, HID_KEY_BRACKET_LEFT, HID_KEY_P, U136, U137,
    U226, U225, U218, U224, U217, U219, U214, U215, U216, U213, U223, U220,
    U229, U228, U233, U235, U232, U236, U231, U230, U227, U234, U221, U222, NUM_KEYS
};

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






void readerInit()   {
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
