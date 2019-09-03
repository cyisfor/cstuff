#include <stdbool.h>
#include <stdlib.h> // size_t

#define BASE_Q -2

size_t int_to_base(char s[], size_t space, unsigned int n, int base);
size_t itoa(char s[], size_t space, unsigned int n);

size_t double_to_base(char s[], size_t space, double n, int base);
size_t dtoa(char s[], size_t space, double n);
