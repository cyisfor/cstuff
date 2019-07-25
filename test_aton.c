#include "aton.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
	void testint(string src, int base) {
		size_t end = 0;
		printf("%ld and then (", strtol(src, &end, base));
		fwrite(src.base + end, src.len - end, 1, stdout);
		fputc(')',stdout);
		fputc('\n',stdout);
	}
	testint(LITSTR("0qBQQq"), BASE_Q);
	testint(LITSTR("BQQq"), BASE_Q);
	testint(LITSTR("BQQFOOPS"), BASE_Q);
	testint(LITSTR("BQQq|\bFOOPS"), BASE_Q);
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
	testdouble(LITSTR("QF.T hi there"), BASE_Q);
		
    return 0;
}
