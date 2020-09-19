#include <stdio.h>
#include <stdint.h>

void longToChar(char x[], long numbers[])
{
  // looping through each of the eight numbers
  for (int i = 0; i < 8; i++)
  {
    long number = numbers[i]; // get the current number
    for (int j = 0; j < 8; j++)
    {                                       // looping through i*8 + 8 char position
      int rightShiftBy = (7 - j) * 8;       // how much we need to right shit the number
      int oneByte = number >> rightShiftBy; // right shift the number to get the reqd 8 bits in LSB
      x[i * 8 + j] = oneByte;               // finally, store that byte in the appropriate place
    }
  }
}

void intToChar(char x[], int numbers[])
{
  // looping through each of the sixteen numbers
  for (int i = 0; i < 16; i++)
  {
    int number = numbers[i]; // get the current number
    for (int j = 0; j < 4; j++)
    {                                       // looping through i*4 + 8 char position
      int rightShiftBy = (3 - j) * 8;       // how much we need to right shit the number
      int oneByte = number >> rightShiftBy; // right shift the number to get the reqd 8 bits in LSB
      x[i * 4 + j] = oneByte;               // finally, store that byte in the appropriate place
    }
  }
}

int main() {
  char x[64];

  long numbers64[8];
  int numbers32[16];

  for (int i = 0; i < 8; i++) { numbers64[i] = i; } // fill with arbitrary longs
  for (int i = 0; i < 16; i++) { numbers32[i] = i; } // fill with arbitrary ints

  longToChar(x, numbers64);
  intToChar(x, numbers32);

  return 0;
}
