/**
  @file codepoints.h
  @author David Lovato
  This file holds the function protoypes for any functions used in
  other programs.  It also holds the information about the
  Code Point Struct.
*/

#include <stdbool.h>

/**
  This function will open the unicode.txt file and
  read all the code point descriptions into a data structure
  (the code point table, described below). If it can't open
  the  unicode.txt file, it will print an appropriate error
  message to standard error and exit unsuccessfully.
*/
void loadTable(void);

/**
  This function will binary search the code point table for the hex value given
  by the input file.  Given the numeric value of a code point,
  this function will print a line to standard output giving the
  name of the code (and return true). If the given code isn't
  in the code point table, it will return false.
  @param code code to look for in code point table
  @return bool of whether or not it could be found
*/
bool reportTable(unsigned int code);

/**
  This function will free the memory for the codepoint table.
*/
void freeTable(void);
