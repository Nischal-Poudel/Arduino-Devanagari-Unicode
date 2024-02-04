#include <Devanagari.h>

Devanagari nepali;

void setup()
{
  // Serial.begin(9600);
  nepali.begin(9600, false); // baudrate, print serial message from devanagari library
}

void loop()
{
  int *decodedcodepoint;
  int arraylength;
  decodedcodepoint = nepali.decodeUnicode("सम्झिन्छौ रे, अझै पनि");
  arraylength = nepali.codepointlength();

  Serial.print("Decoded Unicode: ");
  for (int i = 0; i < arraylength; i++)
  {
    Serial.print(decodedcodepoint[i]);
    Serial.print(", ");
  }
  Serial.print("\nLength: " + String(arraylength) + "\n");
  delay(2000);
}