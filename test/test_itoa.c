#include "itoa.h"
#include "mystring.h"
#include <string.h> // memcmp


int main(int argc, char *argv[])
{
    char buf[0x10];
	size_t amt = itoa(buf, 0x10, 123456);
	if(amt != 6) return 1;
	if(0 != memcmp(buf, LITLEN("123456"))) return 2;

	amt = int_to_base(buf, 0x10, 22, BASE_Q);
	if(amt != 2) return 3;
	if(0 != memcmp(buf, LITLEN("BS"))) return 4;

	amt = int_to_base(buf,0x10,0x7EA7B00B, 0x10);
	if(amt != 8) return 5;
	if(0 != memcmp(buf, LITLEN("7EA7B00B"))) return 6;

	amt = double_to_base(buf, 0x10, 1.234, 10);
	printf("uh (%.*s)\n",amt, buf);
	return 23;
    return 0;
}
