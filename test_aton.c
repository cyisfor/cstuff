#include "aton.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
	size_t end = 0;
	string src = LITSTR("4.56 ashthast");
    printf("%lf and then (", strtod(src, &end, 0));
	fwrite(src.base + end, src.len - end, 1, stdout);
	fputc(')',stdout);
	fputc('\n',stdout);
    return 0;
}
