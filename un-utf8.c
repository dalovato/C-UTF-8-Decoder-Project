/**
  @file un-utf8.c
  @author David Lovato
  This file will open and read from the input file,
  detect errors and use the codepoints component to
  print the names of valid codepoints.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "codepoints.h"

/** This is used for finding the beginning of a codepoint */
#define THREE_BYTES 3

/** This is used for finding the beginning of a codepoint */
#define FOUR_BYTES 4

/** Number for significant bytes in four byte character */
#define SIG_BITS_FOUR_BYTES 16

/** Number for significant bytes in three byte character */
#define SIG_BITS_THREE_BYTES 11

/** Number for significant bytes in twoo byte character */
#define SIG_BITS_TWO_BYTES 7

/** This is used for seeing if a binary code point is all 0's */
#define EMPTY_BIN 0x000000

/** This macro is useful for masking and seeing if we are reading in
    a second, third, or fourth byte in a multibyte character
*/
#define MULTI_BYTE_MASK 0x80

/** This is the mask for the first byte of a four byte character */
#define FOURTH_BYTE_MASK 0x0F

/** This is the mask for the second/third/fourth bytes in multibyte char */
#define CONSEC_BYTE_MASK 0x3F

/** This is the mask for the first byte in two byte character */
#define TWO_BYTE_MASK 0x1F

/** This is the mask used to detect if we have a four byte character */
#define FOUR_BYTE_CHAR_MASK 0xF0

/** This is the mask used to detect if we have a three byte character */
#define THREE_BYTE_CHAR_MASK 0xE0

/** This is the mask used to detect if we have a two byte character */
#define TWO_BYTE_CHAR_MASK 0xC0

/** This is the mask used to detect if we have a two byte character */
#define ONE_BYTE_CHAR_MASK 0x00


/**Global variable for location, since I can't use helper methods without
   making a global variable for location.
*/
int location;

/**
  This function will check to make sure our two byte code isn't overlong
  encoded.
  @param hex, the two byte code
  @return false if overencoded
*/
static bool checkTwoByteEncoding(unsigned int hex)
{
  unsigned int temp = hex;
  temp = temp >> SIG_BITS_TWO_BYTES;
  temp = temp << SIG_BITS_TWO_BYTES;
  if ((temp | EMPTY_BIN) == EMPTY_BIN) {
    return false;
  }
  return true;
}

/**
  This function will check to make sure our three byte code isn't overlong
  encoded.
  @param hex, the 3 byte code
  @return false if overencoded
*/
static bool checkThreeByteEncoding(unsigned int hex)
{
  unsigned int temp = hex;
  temp = temp >> SIG_BITS_THREE_BYTES;
  temp = temp << SIG_BITS_THREE_BYTES;
  //check to see if we have high order bits in the 16 to 21 bit place
  if ((temp | EMPTY_BIN) == EMPTY_BIN) {
    return false;
  }
  temp = hex;
  temp = temp >> SIG_BITS_TWO_BYTES;
  temp = temp << SIG_BITS_TWO_BYTES;
  if ((temp | EMPTY_BIN) == EMPTY_BIN) {
    return false;
  }
  return true;
}


/**
  This function will check to make sure our four byte code isn't overlong
  encoded.
  @param hex, the four byte code
  @return false if overencoded
*/
static bool checkFourByteEncoding(unsigned int hex)
{
  unsigned int temp = hex;
  temp = temp >> SIG_BITS_FOUR_BYTES;
  temp = temp << SIG_BITS_FOUR_BYTES;
  //check to see if we have high order bits in the 16 to 21 bit place
  if ((temp | EMPTY_BIN) == EMPTY_BIN) {
    return false;
  }
  temp = hex;
  temp = temp >> SIG_BITS_THREE_BYTES;
  temp = temp << SIG_BITS_THREE_BYTES;
  if ((temp | EMPTY_BIN) == EMPTY_BIN) {
    return false;
  }
  temp = hex;
  temp = temp >> SIG_BITS_TWO_BYTES;
  temp = temp << SIG_BITS_TWO_BYTES;
  if ((temp | EMPTY_BIN) == EMPTY_BIN) {
    return false;
  }
  return true;
}

/**
  This function handles reading four byte character codes.
  @param unsigned int code, the first byte we read
  @param input file stream
  @return false if incomplete input
*/
static bool fourBytes(unsigned int code, FILE *input)
{
  location++;
  unsigned int bit2 = 0;
  bit2 = fgetc(input);
  location++;
  if (bit2 != EOF) {
    if (((bit2) & (MULTI_BYTE_MASK)) == MULTI_BYTE_MASK) {
      unsigned int bit3 = 0;
      bit3 = fgetc(input);
      location++;
      if (bit3 != EOF) {
        if (((bit3) & (MULTI_BYTE_MASK)) == MULTI_BYTE_MASK) {
          unsigned int bit4 = 0;
          bit4 = fgetc(input);
          location++;
          if (bit4 != EOF) {
            if (((bit4) & (MULTI_BYTE_MASK)) == MULTI_BYTE_MASK) {
              code &= FOURTH_BYTE_MASK;
              bit2 &= CONSEC_BYTE_MASK;
              bit3 &= CONSEC_BYTE_MASK;
              bit4 &= CONSEC_BYTE_MASK;
              unsigned int hex = (code << (SIG_BITS_TWO_BYTES - 1)) | bit2;
              hex = (hex << (SIG_BITS_TWO_BYTES - 1));
              hex |= bit3;
              hex = (hex << (SIG_BITS_TWO_BYTES - 1));
              hex |= bit4;
              if (!checkFourByteEncoding(hex)) {
                fprintf(stderr, "Invalid encoding: 0x%X at %d\n", hex, (location - FOUR_BYTES));
              } else if (!reportTable(hex)) {
                fprintf(stderr, "Unknown code: 0x%X at %d\n", hex, (location - FOUR_BYTES));
              }
              return true;
            } else {
              fprintf(stderr, "Invalid byte: 0x%X at %d\n", bit4, (location - 1));
              return true;
            }
          } else {
            fprintf(stderr, "Incomplete code at %d\n", (location - FOUR_BYTES));
            return false;
          }
        } else {
          fprintf(stderr, "Invalid byte: 0x%X at %d\n", bit3, (location - 1));
          bit3 = fgetc(input); //do nothing
          location++;
          return true;
        }
      } else {
        fprintf(stderr, "Incomplete code at %d\n", (location - THREE_BYTES));
        return false;
      }
    } else {
      fprintf(stderr, "Invalid byte: 0x%X at %d\n", bit2, (location - 1));
      bit2 = fgetc(input); //do nothing with these
      location++;
      bit2 = fgetc(input); //do nothing with these
      location++;
      return true;
    }
  } else {
    fprintf(stderr, "Incomplete code at %d\n", (location - 2));
    return false;
  }
}

/**
  This function will handle 3 byte character codes.
  @param unsigned int code, first byte
  @param location, number of bytes read
  @param input file stream
  @return false if incomplete input
*/
static bool threeBytes(unsigned int code, FILE *input)
{
  location++;
  unsigned int bit2 = 0;
  bit2 = fgetc(input);
  location++;
  if (bit2 != EOF) {
    if (((bit2) & (MULTI_BYTE_MASK)) == MULTI_BYTE_MASK) {
      unsigned int bit3 = 0;
      bit3 = fgetc(input);
      location++;
      if (bit3 != EOF) {
        if (((bit3) & (MULTI_BYTE_MASK)) == MULTI_BYTE_MASK) {
          code &= FOURTH_BYTE_MASK;
          bit2 &= CONSEC_BYTE_MASK;
          bit3 &= CONSEC_BYTE_MASK;
          unsigned int hex = (code << (SIG_BITS_TWO_BYTES - 1)) | bit2;
          hex = (hex << (SIG_BITS_TWO_BYTES - 1));
          hex |= bit3;
          if (!checkThreeByteEncoding(hex)) {
            fprintf(stderr, "Invalid encoding: 0x%X at %d\n", hex, (location - THREE_BYTES));
          } else if (!reportTable(hex)) {
            fprintf(stderr, "Unknown code: 0x%X at %d\n", hex, (location - THREE_BYTES));
          }
          return true;
        } else {
          fprintf(stderr, "Invalid byte: 0x%X at %d\n", bit3, (location - 1));
          return true;
        }
      } else {
        fprintf(stderr, "Incomplete code at %d\n", (location - THREE_BYTES));
        return false;
      }
    } else {
      fprintf(stderr, "Invalid byte: 0x%X at %d\n", bit2, (location - 1));
      bit2 = fgetc(input); //do nothing
      location++;
      return true;
    }
  } else {
    fprintf(stderr, "Incomplete code at %d\n", (location - 2));
    return false;
  }
}

/**
  This function is for handling two byte character codes.
  @param unsigned int code, first byte
  @param location of latest byte read
  @param input file stream
  @return new location
*/
static int twoBytes(unsigned int code, FILE *input)
{
  location++;
  unsigned int bit2 = 0;
  bit2 = fgetc(input);
  location++;
  if (bit2 != EOF) {
    if (((bit2) & (MULTI_BYTE_MASK)) == MULTI_BYTE_MASK) {
      code &= TWO_BYTE_MASK;
      bit2 &= CONSEC_BYTE_MASK;
      unsigned int hex = (code << (SIG_BITS_TWO_BYTES - 1)) | bit2;
      if (!checkTwoByteEncoding(hex)) {
        fprintf(stderr, "Invalid encoding: 0x%X at %d\n", hex, (location - 2));
      } else if (!reportTable(hex)) {
        fprintf(stderr, "Unknown code: 0x%X at %d\n", hex, (location - 2));
      }
      return true;
    } else {
      fprintf(stderr, "Invalid byte: 0x%X at %d\n", bit2, (location - 1));
      return true;
    }
  } else {
    fprintf(stderr, "Incomplete code at %d\n", (location - 2));
    return false;
  }
}

/**
  This is the main function which read and open
  the input file and call other functions
  to undo the utf8 coding.
  @param argc, the number of argument commands
  @param argv, an array of arguments
  @return exit status
*/
int main(int argc, char *argv[])
{
  if (argc != 2) {
    fprintf(stderr, "usage: un-utf8 <input-file>\n");
    exit(1);
  } else {
    FILE *input = fopen(argv[1], "rb");
    if (!input) {
      fprintf(stderr, "Can't open file: %s\n", argv[1]);
      fprintf(stderr, "usage: un-utf8 <input-file>\n");
      exit(1);
    }
    loadTable();
    unsigned int code = 0;
    code = fgetc(input);
    while (code != EOF) {
      if (((code) & (FOUR_BYTE_CHAR_MASK)) == FOUR_BYTE_CHAR_MASK) {
        if (!fourBytes(code, input)) {
          break;
        }
      } else if (((code) & (THREE_BYTE_CHAR_MASK)) == THREE_BYTE_CHAR_MASK) {
        if (!threeBytes(code, input)) {
          break;
        }
      } else if (((code) & (TWO_BYTE_CHAR_MASK)) == TWO_BYTE_CHAR_MASK) {
        if (!twoBytes(code, input)) {
          break;
        }
      } else if (((code) & (MULTI_BYTE_MASK)) == ONE_BYTE_CHAR_MASK) {
        if (!reportTable(code)) {
          fprintf(stderr, "Unknown code: 0x%X at %d\n", code, location);
        }
        location++;
      } else {
        fprintf(stderr, "Invalid byte: 0x%X at %d\n", code, location);
        location++;
      }
      code = fgetc(input);
    }
    freeTable();
    fclose(input);
    return EXIT_SUCCESS;
  }
}
