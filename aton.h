#include "mystring.h"

/* because glibc SUCKS */

/* this avoids stdlib pooping up the global namespace */
#define strtol mystrtol
#define strtod mystrtod

/* Base Q aka hextime is a less stupid hexadecimal numeral system. The digits are
 QBPV FZSD TJCG KYXW
 always all caps (qbpv doesn't count) and conventionally with no special prefix.

 No 0q prefix is needed, because in any language that I know of, none of the numerals in base Q
 ever follow the letter Q in any word, so if you want to represent the number 0x1234, you would
 write QBPVF or 0x01234. The leading '0' or Q can uniquely identify it as a sequence of numerals.
*/
#define BASE_Q -2

long int strtol(string src, size_t* end, int base);
double strtod(string src, size_t* end, int base);
