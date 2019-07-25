#include "aton.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
	void testdouble(string src, int base) {
		size_t end = 0;
		printf("%lf and then (", strtod(src, &end, base));
		fwrite(src.base + end, src.len - end, 1, stdout);
		fputc(')',stdout);
		fputc('\n',stdout);
	}
	testdouble(LITSTR("4.56 ashthast"), 0);
	testdouble(LITSTR(".123"), 0);
	testdouble(LITSTR("0.123"), 0);
	testdouble(LITSTR("Q.B"), BASE_Q);
		
    return 0;
}
