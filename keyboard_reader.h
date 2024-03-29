#ifndef _KBD_READER_H_
#define _KBD_READER_H_

#include "pico/stdlib.h"

enum inPin{GP9, GP10, GP11, GP16, GP17, GP18, GP19, GP20, GP21, GP22, GP26, GP27, NUM_IN_PINS};
enum outPin{GP0, GP1, GP2, GP3, GP4, GP5, GP6, GP7, GP8, NUM_OUT_PINS};
enum keys{
    U167, U141, U166, U142, U165, U143, U164, U144, U129, U130, U163, U145, U162, U146,
    U132, U133, U161, U147, U160, U148, U159, U149, U158, U150, U157, U151, 
    U209, U212, U183, U184, U169, U170, U155, U156, U154, U152, U211, U210,
    U198, U197, U195, U196, U181, U182, U168, U140, U126, U127, U200, U199, 
    U193, U194, U179, U180, U128, U202, U201, U191, U192, U177, U178, 
    U131, U204, U203, U189, U190, U175, U176, U206, U205, U187, U188, 
    U173, U174, U134, U135, U208, U207, U185, U186, U171, U172, U136, U137,
    U226, U225, U218, U224, U217, U219, U214, U215, U216, U213, U223, U220,
    U229, U228, U233, U235, U232, U236, U231, U230, U227, U234, U221, U222, NUM_KEYS
};


uint8_t getCodeFromEnum(uint8_t key);
void readerInit();
void readerGetPressedKeys(bool keyArray[NUM_KEYS]);

#endif