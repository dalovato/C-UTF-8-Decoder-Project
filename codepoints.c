/**
  @file codepoints.c
  @author David Lovato
  This component will read and maintain the list of codepoints,
  where each codepoint has a numeric code and a string name.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/** Description for Code Point is only 90 chars long */
#define SCRIPT_LEN 91
/** Initial capacity of code point list is 5 */
#define INIT_CAP 5

typedef struct CodePointTag {
  /** unique 1 to 4 byte identifier for the code point */
  unsigned int code;
  
  /** description of the code point */
  char description[SCRIPT_LEN];
} CodePoint;

/** Representation for a list of CodePoints, an arbitrary-length list. */
typedef struct {
  /** Resizable array of pointers to Code Points. */
  CodePoint **codepoints;
  
  /** Current number of items on the list. */
  int length;
  
  /** Current capacity of the list, how many pointers we have room for. */
  int capacity;
} CodePointList;


/** Pointer to resizeable array of code point pointers */
static CodePointList *list;

/**
  This function will compare two CodePoint structs and return
  which CodePoint should go before the other.
  @param void *p, a pointer to a pointer to a CodePoint
  @param void *q, a pointer to a pointer to a CodePoint
  @return int, if 0, they are equal, if -1, then put p first, else q
*/
int comparator(const void *p, const void *q)
{

  CodePoint *point1 = *(CodePoint **)p;
  CodePoint *point2 = *(CodePoint **)q;
  
  unsigned int l = point1->code;
  unsigned int r = point2->code;
  
  if (l < r) {
    return -1;
  } else if (l > r) {
    return 1;
  } else {
    return 0;
  }
}

/**
  This function will open the unicode.txt file and
  read all the code point descriptions into a data structure
  (the code point table, described below). If it can't open
  the  unicode.txt file, it will print an appropriate error
  message to standard error and exit unsuccessfully.
*/
void loadTable()
{
  FILE *unicode = fopen("unicode.txt", "r");

  //if file can't open, exit unsuccessfully
  if (!unicode) {
    fprintf(stderr, "Can't open file: unicode.txt\n");
    exit(1);
  }
  
  //initialize static global variable
  list = (CodePointList *)malloc(sizeof(CodePointList));
  
  //let's initialize the code point list length and capacity
  //list is already initialized as static global variable
  list->length = 0;
  list->capacity = INIT_CAP;
  list->codepoints = malloc(sizeof(CodePoint *) * list->capacity);
  
  //int to hold hex value and char array for description 90 chars long
  unsigned int hex_val = 0;
  char script[SCRIPT_LEN];
  
  while (fscanf(unicode, "%x %[^\n]", &hex_val, script) == 2) {
    CodePoint *c = (CodePoint *)malloc(sizeof(CodePoint));
    c->code = hex_val;
    strcpy(c->description, script);

    /** this code came from my list.c in project 3 */
    if (list->length >= (list->capacity - 1)) {
      list->capacity *= 2;
      list->codepoints = realloc(list->codepoints,
                                  (sizeof(int)*2 + sizeof(CodePoint *) * list->capacity));
    }
    list->codepoints[list->length++] = c;
  }

  qsort(list->codepoints, list->length, sizeof(CodePoint *), comparator);
  fclose(unicode);
}

/**
  This function will binary search the code point table for the hex value given
  by the input file.  Given the numeric value of a code point,
  this function will print a line to standard output giving the
  name of the code (and return true). If the given code isn't
  in the code point table, it will return false.
  @param code code to look for in code point table
  @return bool of whether or not it could be found
*/
bool reportTable(unsigned int code)
{
  int list_begin = 0;
  int list_end = list->length;
  int list_mid_pt = ((list_begin + list_end) / 2);
  while (code != list->codepoints[list_mid_pt]->code) {
    if (code > list->codepoints[list_mid_pt]->code) {
      list_begin = list_mid_pt;
      int temp = ((list_begin + list_end) / 2);
      
      //check to make sure we aren't stuck in between values
      if (list_mid_pt == temp) {
        return false;
      }
      
      list_mid_pt = ((list_begin + list_end) / 2);
    } else if (code < list->codepoints[list_mid_pt]->code) {
      list_end = list_mid_pt;
      int temp = ((list_begin + list_end) / 2);
      
      //check to make sure we aren't stuck in between values
      if (list_mid_pt == temp) {
        return false;
      }
      
      list_mid_pt = ((list_begin + list_end) / 2);
    }
  }
  printf("%s\n", list->codepoints[list_mid_pt]->description);
  return true;
}


/**
  This function will free the memory for the codepoint table.
*/
void freeTable(void)
{
  for (int i = 0; i < list->length; i++) {
    free(list->codepoints[i]);
  }
  free(list->codepoints);
  free(list);
}
