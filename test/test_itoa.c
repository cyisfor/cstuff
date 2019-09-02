#include "itoa.h"

int main(int argc, char *argv[])
{
    char buf[0x10];
	size_t amt = itoa(buf, 0x10, 123456);
	if(amt != 6) return 1;
	if(0 != memcmp(buf, LITLEN("123456"))) {
		return 2;
	}
	amt = int_to_base(buf, 0x10, 22, BASE_Q);
	if(amt != 2) return 3;
	if(0 != memcmp(buf, LITLEN("BS"))) return 4;

	amt = int_to_base(buf,0x10,0x
    return 0;
}
