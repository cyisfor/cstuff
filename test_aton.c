#include "aton.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
	size_t end = 0;
    printf("%lf", strtod(LITSTR("4.56"), &end, 0));
    return 0;
}
