#ifndef DEVANAGARI_H
#define DEVANAGARI_H

#include "Arduino.h"

#define UTF8_END -1
#define UTF8_ERROR -2

class Devanagari
{
public:
    // constructor
    //  Devanagari(int w, int h); // Display Width and Height
    uint16_t *decodeUnicode(char *string);
    void begin(int baudrate = 115200, bool debug = true);
    int codepointlength();

private:
    int _debugmessage;
    uint16_t *_codepointarray = 0;
    int _codepoint_length = 0;
    int _disp_width;
    int _disp_height;
    int _the_index = 0;
    int _the_length = 0;
    int _the_char = 0;
    int _the_byte = 0;
    char *_the_input;

    int utf8_decode_at_byte();
    int utf8_decode_at_character();
    void utf8_decode_init(char p[], int length);
    int utf8_decode_next();
    int get();
    int cont();
    uint16_t *removezerofromarray();
};

#endif
