#include <Arduino.h>
#include <PxMatrix.h>
#include <Adafruit_GFX.h>
#include <Devanagari.h> //This font library that decodes the unicode and handles print rendering

/*Include the devanagari font library.
 * This library contains the main glyphs for the devanagari chars
 * standarized by Unicode Standard 14.0
 */
#include <Fonts/BrahmaLilaBold11pt14b.h>
/* Include the 'Special chars' devanagari font library
 * This font library contains the special devanagari chars and half
 * form of consonants.
 */

#include <Fonts/BrahmaLilaBoldSpecialChars11pt14b.h>
#include <Fonts/FreeSansBold9pt7b.h> //Normal ASCII characters glyph

// Pins for LED MATRIX
#define P_LAT 16 // 22
#define P_A 19
#define P_B 2 // 23
#define P_C 18
#define P_D 4 // 5
#define P_E 15
#define P_OE 5

#define matrix_width 128
#define matrix_height 64

PxMATRIX display(matrix_width, matrix_height, P_LAT, P_OE, P_A, P_B, P_C, P_D, P_E);

// Initiate the Devanagari Library
Devanagari nepali;

long looptimer;

void setup()
{
  // Initialize Serial port
  nepali.begin(115200, false); // baudrate, display library messages
  while (!Serial)
    continue;

  display.begin(32); // 1/32 scan display
  display.clearDisplay();
  looptimer = millis();
}

void printUnicode(char *string)
{
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
    /*If the Unicode code point is ASCII,
    *Then include the English ASCII Font Library
    *This will enable to print the ASCII and Devanagari font all at once in a single string.

    * If the unicode code point is in 0900 to 097F hex range
    * Include the devanagari font library

    * If the unicode code point is in 518 to 597 hex range
    * Include the 'Special chars' devanagari font library
    * This font library contains the special devanagari chars and half
    * form of consonants
    */
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
    // print the char on the display.
    display.write(unicodecp);
    display.display();

    // Display the unicode codepoint into the serial Monitor.
    Serial.print(unicodecp);
    Serial.println(", ");
  }
  Serial.println("String: " + String(string) + "Length: " + String(arraylength) + "\n");
}

void loop()
{
  looptimer = millis();

  display.setFont(); // Import font from font library
  display.setTextColor(display.color565(200, 250, 50));
  display.setCursor(5, 10);
  printUnicode("निश्चल पौडेल");
  display.setCursor(15, 35);
  printUnicode("निस्चल, Paudel");
  display.setCursor(15, 55);
  printUnicode("स्मार्ट LED बोर्ड");
  display.display(20);

  Serial.print("\nLoop run time: ");
  Serial.println(millis() - looptimer);
}
