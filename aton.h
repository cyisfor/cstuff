#include "mystring.h"

/* because glibc SUCKS */

/* this avoids stdlib pooping up the global namespace */
#define strtol mystrtol
#define strtod mystrtod

#define BASE_Q -2

long int strtol(string src, size_t* end, int base);
double strtod(string src, size_t* end, int base);
