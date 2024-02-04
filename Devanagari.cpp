/* utf8_decode.c */

/* 2016-04-05 */

/*
Copyright (c) 2005 JSON.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

The Software shall be used for Good, not Evil.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/*
This code is Modified by Nischal Poudel on september, 2022 to
espiscially render devanagari and ASCII unicodes. This includes a
full print rendering system to display all devanagari chars perectly
no matter how complex their structure is.
* The user must include 'BrahmaLilaBold11pt14b.h'
* and 'BrahmaLilaBoldSpecialChars11pt14b.h' . The brhamalila bold includes the glyphs
* of all unicode standard chars and is made following Unicode standard V14.
* More info here: https://www.unicode.org/charts/PDF/U0900.pdf
* There are specific set of rules for rendering the devanagari font on the display
* and the rule set is required to develop the print rendering engine. This print
* redering system uses this standard of Unicode V.14 for its processing of the fonts.
* More info of this here: https://www.unicode.org/versions/Unicode14.0.0/ch12.pdf
* The Unicode codeblock alone doesnot contain all unicode glyphs and majority of the
* combined char glyphs in other spaces by the rendering device itself. Here for this to
* work on all adafruit_GFX supported displays, another font library has been created
* by thhe name 'BrahmaLilaBoldSpecialChars11pt14b.h' from hex (52B-569) decimal(1323 - 1385).
* so, Both libraries must be included to render full devanagari words and sentences.
* The 'commas' full stop, brackets are the form of ASCII glyphs so, 'FreeSansBold9pt7b.h'
* by the adafruit fonts is also included to render english and devanagari seamlessly.

also, the below mentioned function should be made in every sketch utilizing this library
that slects the suitable font to render the unicode code block.
// the arduino function sketch is as follows.

void printUnicode(char* string){
  uint16_t *decodedcodepoint;
  int arraylength;
  int codepoint;
  int prevcodepoint;

  decodedcodepoint = nepali.decodeUnicode(string);
  arraylength = nepali.codepointlength();


//  Serial.print("Decoded Unicode: ");

  for (int i = 0; i < arraylength; i++)
  {
    int16_t unicodecp = decodedcodepoint[i];


//-------------------------------------------------------------------//
*If the Unicode code point is ASCII,
*Then include the English ASCII Font Library
*This will enable to print the ASCII and Devanagari font all at once in a single string.

* If the unicode code point is in 0900 to 097F hex range
* Include the devanagari font library

* If the unicode code point is in 518 to 597 hex range
* Include the 'Special chars' devanagari font library
* This font library contains the special devanagari chars and half
* form of consonants
*
if (unicodecp < 1323)
{
    display.setFont(&FreeSansBold9pt7b);
}
if (unicodecp >= 1323 && unicodecp <= 1385)
{
    display.setFont(&BrahmaLilaBoldSpecialChars11pt14b);
}
if (unicodecp >= 2304 && unicodecp <= 2431)
{
    display.setFont(&BrahmaLilaBold11pt14b);
}

display.write(unicodecp);
display.display();

// Display the unicode codepoint into the serial Monitor.
Serial.print(string);
Serial.println("");
Serial.print(unicodecp);
Serial.print(", ");
}

Serial.print("\nLength: " + String(arraylength) + "\n");
}

/---------------------------------------------------------------------------/
And this function can be called by
 printUnicode("Nischal, पौडेल");
 this can used in the place of diisplay.print(); in the standard adafruit sketch.
*/

#include "Devanagari.h"
#include "Arduino.h"
//#include <Adafruit_GFX.h>
/*
    Very Strict UTF-8 Decoder

    UTF-8 is a multibyte character encoding of Unicode. A character can be
    represented by 1-4 bytes. The bit pattern of the first byte indicates the
    number of continuation bytes.

    Most UTF-8 decoders tend to be lenient, attempting to recover as much
    information as possible, even from badly encoded input. This UTF-8
    decoder is not lenient. It will reject input which does not include
    proper continuation bytes. It will reject aliases (or suboptimal
    codings). It will reject surrogates. (Surrogate encoding should only be
    used with UTF-16.)

    Code     Contination Minimum Maximum
    0xxxxxxx           0       0     127
    10xxxxxx       error
    110xxxxx           1     128    2047
    1110xxxx           2    2048   65535 excluding 55296 - 57343
    11110xxx           3   65536 1114111
    11111xxx       error
*/

/*
    Get the next byte. It returns UTF8_END if there are no more bytes.
*/
int Devanagari::get()
{
    int c;
    if (_the_index >= _the_length)
    {
        return UTF8_END;
    }
    c = _the_input[_the_index] & 0xFF;
    _the_index += 1;
    return c;
}

/*
    Get the 6-bit payload of the next continuation byte.
    Return UTF8_ERROR if it is not a contination byte.
*/
int Devanagari::cont()
{
    int c = get();
    return ((c & 0xC0) == 0x80)
               ? (c & 0x3F)
               : UTF8_ERROR;
}

/*
    Initialize the UTF-8 decoder. The decoder is not reentrant,
*/
void Devanagari::utf8_decode_init(char p[], int length)
{
    _the_index = 0;
    _the_input = p;
    _the_length = length;
    _the_char = 0;
    _the_byte = 0;
    _codepoint_length = 0; // The returning length of array
}

/*
    Get the current byte offset. This is generally used in error reporting.
*/
int Devanagari::utf8_decode_at_byte()
{
    return _the_byte;
}

/*
    Get the current character offset. This is generally used in error reporting.
    The character offset matches the byte offset if the text is strictly ASCII.
*/
int Devanagari::utf8_decode_at_character()
{
    return (_the_char > 0)
               ? _the_char - 1
               : 0;
}

/*
    Extract the next character.
    Returns: the character (between 0 and 1114111)
         or  UTF8_END   (the end)
         or  UTF8_ERROR (error)
*/
int Devanagari::utf8_decode_next()
{
    int c;  /* the first byte of the character */
    int c1; /* the first continuation character */
    int c2; /* the second continuation character */
    int c3; /* the third continuation character */
    int r;  /* the result */

    if (_the_index >= _the_length)
    {
        return _the_index == _the_length ? UTF8_END : UTF8_ERROR;
    }
    _the_byte = _the_index;
    _the_char += 1;
    c = get();
    /*
        Zero continuation (0 to 127)
    */
    if ((c & 0x80) == 0)
    {
        return c;
    }
    /*
        One continuation (128 to 2047)
    */
    if ((c & 0xE0) == 0xC0)
    {
        c1 = cont();
        if (c1 >= 0)
        {
            r = ((c & 0x1F) << 6) | c1;
            if (r >= 128)
            {
                return r;
            }
        }

        /*
            Two continuations (2048 to 55295 and 57344 to 65535)
        */
    }
    else if ((c & 0xF0) == 0xE0)
    {
        c1 = cont();
        c2 = cont();
        if ((c1 | c2) >= 0)
        {
            r = ((c & 0x0F) << 12) | (c1 << 6) | c2;
            if (r >= 2048 && (r < 55296 || r > 57343))
            {
                return r;
            }
        }

        /*
            Three continuations (65536 to 1114111)
        */
    }
    else if ((c & 0xF8) == 0xF0)
    {
        c1 = cont();
        c2 = cont();
        c3 = cont();
        if ((c1 | c2 | c3) >= 0)
        {
            r = ((c & 0x07) << 18) | (c1 << 12) | (c2 << 6) | c3;
            if (r >= 65536 && r <= 1114111)
            {
                return r;
            }
        }
    }
    return UTF8_ERROR;
}

uint16_t *Devanagari::decodeUnicode(char *string)
{

    size_t len = 0;
    uint16_t codepointsToPrint[1200]; //= (uint16_t *)calloc(strlen(string), 2); // malloc(strlen(string) * 2);

    if (codepointsToPrint == NULL)
        return NULL;
    utf8_decode_init(string, strlen(string));
    do
    {
        int c = utf8_decode_next();
        if (c == UTF8_END || c == UTF8_ERROR)
            break;
        codepointsToPrint[len++] = (uint16_t)c;
    } while (1);

    for (size_t i = 0; i < len; i++)
    {
        /*
         *To print some combined chars like 'म्र', 'प्र', 'ट्र'. They are formed
         * by a consonant sound followed by a 0x94D '्' and then  0x930 'र'.
         * Use cases are तिम्रो, राष्ट्र, क्रान्ति, etc.
         */
        if ((codepointsToPrint[i] >= 2325 && codepointsToPrint[i] <= 2361) && codepointsToPrint[i + 1] == 2381 && codepointsToPrint[i + 2] == 2352)
        {
            //  0x915 'क',  0x92B 'फ', 0x936 'श' , 0x91F 'ट', 0x920 'ठ'
            if (!(codepointsToPrint[i] == 2325 || codepointsToPrint[i] == 2347 || codepointsToPrint[i] == 2358 || codepointsToPrint[i] == 2335 || codepointsToPrint[i] == 2336 || codepointsToPrint[i] == 2340 || codepointsToPrint[i] == 2346))
            {
                codepointsToPrint[i + 1] = 1378;
                codepointsToPrint[i + 2] = 0;
            }
            else if (codepointsToPrint[i] == 2325)
            {                                // 0x915 'क'
                codepointsToPrint[i] = 1371; // 0x55B 'क्र'
                codepointsToPrint[i + 1] = 0;
                codepointsToPrint[i + 2] = 0;
                i += 2;
            }
            else if (codepointsToPrint[i] == 2346)
            {                                // 0x92A 'प'
                codepointsToPrint[i] = 1373; // 0x55D 'प्र'
                codepointsToPrint[i + 1] = 0;
                codepointsToPrint[i + 2] = 0;
                i += 2;
            }

            else if (codepointsToPrint[i] == 2347)
            {                                // 0x92B 'फ'
                codepointsToPrint[i] = 1372; // 0x55C 'फ्र'
                codepointsToPrint[i + 1] = 0;
                codepointsToPrint[i + 2] = 0;
                i += 2;
            }
            else if (codepointsToPrint[i] == 2358)
            {                                // 0x936 'श'
                codepointsToPrint[i] = 1365; // 0x555 'श्र'
                codepointsToPrint[i + 1] = 0;
                codepointsToPrint[i + 2] = 0;
                i += 2;
            }
            else if (codepointsToPrint[i] == 2340)
            {                                // 0x924 'त'
                codepointsToPrint[i] = 1363; // 0x553 'त्र'
                codepointsToPrint[i + 1] = 0;
                codepointsToPrint[i + 2] = 0;
                i += 2;
            }

            else if (codepointsToPrint[i] == 2335 || codepointsToPrint[i] == 2336 || codepointsToPrint[i] == 2342)
            {                                    // 0x91F 'ट', 0x920 'ठ', 0x926 'द'
                codepointsToPrint[i + 1] = 1377; // 0x561 "ट्र" ,
                codepointsToPrint[i + 2] = 0;
            }
        }

        /*
         *'क्ष' 0x552 is a Devanagari characters formed by the combination of 3 other devanagari characters.
         * However, this is not in the unicode official glyph list so, its glyph(image data) is stored in
         * another font library created by ourself so 'क्ष' 0x552 is not the official unicode designation
         * but the user designated value where the glyph for the character is stored. The similar thing
         * is done to make it able to display unique devanagari chars.
         */

        // 0x915 'क' + 0x94D '्' + 0x937 'ष' (2359 in decimal) = 'क्ष' 0x552
        if ((codepointsToPrint[i] == 2325 && codepointsToPrint[i + 1] == 2381) && (codepointsToPrint[i + 2] == 2359))
        {
            codepointsToPrint[i] = 1362; //'क्ष' 0x552
            codepointsToPrint[i + 1] = 0;
            codepointsToPrint[i + 2] = 0;
            i += 2;
        }
        // 0x91C 'ज' (2332 in decimal)+ 0x94D '्' + 0x91E 'ञ'(2334 in decimal) = 'ज्ञ' 0x554
        if ((codepointsToPrint[i] == 2332 && codepointsToPrint[i + 1] == 2381) && (codepointsToPrint[i + 2] == 2334))
        {
            codepointsToPrint[i] = 1364; //'ज्ञ' 0x554
            codepointsToPrint[i + 1] = 0;
            codepointsToPrint[i + 2] = 0;
            i += 2;
        }
        // 0x924 'त' (2340 in decimal)+ 0x94D '्' + 0x924 'त'(2334 in decimal) = 'त्त' 0x55E (1374 in decimal)
        if (codepointsToPrint[i] == 2340 && codepointsToPrint[i + 1] == 2381 && codepointsToPrint[i + 2] == 2340)
        {
            codepointsToPrint[i] = 1374;
            codepointsToPrint[i + 1] = 0;
            codepointsToPrint[i + 2] = 0;
            i += 2;
        }
        // 0x930 'र' + 0x941 'ु' = ruapiya
        if (codepointsToPrint[i] == 2352)
        {
            if (codepointsToPrint[i + 1] == 2369)
            {                                // 0x941 'ु'
                codepointsToPrint[i] = 1375; // 0x55F
                codepointsToPrint[i + 1] = 0;
                i++;
            }

            else if (codepointsToPrint[i + 1] == 2370)
            {                                // 0x942 'ू'
                codepointsToPrint[i] = 1376; // 0x560
                codepointsToPrint[i + 1] = 0;
                i++;
            }
        }

        /*
               If there is consonant letter like  // 0x915 'क', 0x916 'ख', 0x917 'ग'
               and there comes a modifier  0x94D '्' which is supposed not be visible but
               is supposed to convert consonant letters into thier half forms.
               */
        if ((codepointsToPrint[i] >= 2325 && codepointsToPrint[i] <= 2361) && codepointsToPrint[i + 1] == 2381)
        {
            codepointsToPrint[i] = codepointsToPrint[i] - 1000;
            codepointsToPrint[i + 1] = 0;
            i++;
        }
    } // End for loop here.

    //  Sync the values
    _codepointarray = codepointsToPrint;
    _codepoint_length = len;

    _codepointarray = removezerofromarray();

    for (int i = 0; i < _codepoint_length; i++)
        codepointsToPrint[i] = _codepointarray[i];

    len = _codepoint_length; // sync the length after 0s are removed

    for (size_t i = 0; i < len; i++)
    {
        // To create half form of 'क्ष्' 0x552, 'त्र' 0x553, 'ज्ञ' 0x554, 'श्र' 0x555
        if ((codepointsToPrint[i] >= 1362 && codepointsToPrint[i] <= 1365) && codepointsToPrint[i + 1] == 2381)
        {
            codepointsToPrint[i] = codepointsToPrint[i] + 4;
            codepointsToPrint[i + 1] = 0;
            i++; // incriment i so that it wont scan i+1 i.e 0;
                 // Remove the zero from the array when introduced.
            _codepointarray = codepointsToPrint;
            _codepointarray = removezerofromarray();

            for (int i = 0; i < _codepoint_length; i++)
                codepointsToPrint[i] = _codepointarray[i];
            len = _codepoint_length; // sync the length after 0s are removed
        }

        /* If there comes the modified form of 'र्' in the codeblock, that is avove chars
                * render it after the next char. like in "गर्व" the half form of 'र्' comes early in
                unicode but is rendered after the next char as seen in example word.
                */

        if (codepointsToPrint[i] == 1352)
        {
            if (codepointsToPrint[i + 2] == 2368 || codepointsToPrint[i + 2] == 2366)
            { // 0x940 'ी' , 0x93E 'ा'
                codepointsToPrint[i] = codepointsToPrint[i + 1];
                codepointsToPrint[i + 1] = codepointsToPrint[i + 2];
                codepointsToPrint[i + 2] = 1352;
                i += 2;
            }
            else
            {
                codepointsToPrint[i] = codepointsToPrint[i + 1];
                codepointsToPrint[i + 1] = 1352;
                i++;
            }
        }

        /*
         * If there comes 0x93F 'ि' in the array, move this one position before,
         * as in devangari, 'ि' is written first before devanagari char.
         */
        if (codepointsToPrint[i] == 2367)
        {
            if ((codepointsToPrint[i - 2] >= 1325 && codepointsToPrint[i - 2] <= 1361) || (codepointsToPrint[i - 2] >= 1366 && codepointsToPrint[i - 2] <= 1369) || codepointsToPrint[i - 1] == 1378)
            {
                codepointsToPrint[i] = codepointsToPrint[i - 1];
                codepointsToPrint[i - 1] = codepointsToPrint[i - 2];
                codepointsToPrint[i - 2] = 2367;
            }
            else
            {

                uint16_t Previous_codepoint;
                Previous_codepoint = codepointsToPrint[i - 1];
                codepointsToPrint[i - 1] = codepointsToPrint[i];
                codepointsToPrint[i] = Previous_codepoint;
            }
        }

        if (_debugmessage != false)
        {
            Serial.print(codepointsToPrint[i]);
            Serial.print(", ");
            Serial.println("");
        }
    }
    codepointsToPrint[len] = 0;

    _codepointarray = codepointsToPrint;
    _codepoint_length = len;

    if (_debugmessage != false)
    {
        Serial.println(string);
        Serial.print("Size of array(bytes): ");
        Serial.println(sizeof(codepointsToPrint));
        Serial.print("\nUnicode Decoding Completed...\n");
    }

    return _codepointarray;
}

void Devanagari::begin(int baudrate, bool debug)
{
    _debugmessage = debug;
    Serial.begin(baudrate);
    while (!Serial)
        continue;

    if (_debugmessage != false)
        Serial.print("The Serial Monitor has initiated: \n");
}
uint16_t *Devanagari::removezerofromarray()
{
    /*The below set of codes will remove any 0 that were present in the
     * codeblock array. This is to manage proper ordering of the glyphs in the below
     *codes for words like 'पश्चिम', 'पूर्व'. 'सि' 'र्व'
     */
    size_t J = 0;
    while (J < _codepoint_length)
    {
        if (_codepointarray[J] == 0)
        {
            if (J < (_codepoint_length - 1))
                for (int K = J; K < (_codepoint_length - 1); K++)
                    _codepointarray[K] = _codepointarray[K + 1];
            _codepoint_length--;
        }
        else
        {
            J++;
        }
    }
    return _codepointarray;
}

int Devanagari::codepointlength()
{
    int __length = _codepoint_length;
    if (__length != 0)
    {
        return __length;
    }
    else
    {
        return 0;
    }
}
