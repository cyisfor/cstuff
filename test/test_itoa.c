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

	amt = double_to_base(buf, 5, 1.235, 10);
	if(amt != 5) return 7;
	if(0 != memcmp(buf, LITLEN("1.235"))) return 8;

	amt = double_to_base(buf, 0x10, 8.0 + 1/16.0 + 1/32.0 + 1/256.0, 0x10);
	if(amt != 4) return 9;
	if(0 != memcmp(buf, LITLEN("8.19"))) return 10;

	amt = double_to_base(buf, 0x10, 8.0 + 1/16.0 + 1/32.0 + 1/256.0, BASE_Q);
	if(amt != 4) return 9;
	if(0 != memcmp(buf, LITLEN("T.BJ"))) return 10;

	amt = double_to_base(buf, 0x10, 13.9374, 0x10);
	if(amt != 14) return 11;
	if(0 != memcmp(buf, LITLEN("D.EFF972474539"))) return 12;

    return 0;
}
