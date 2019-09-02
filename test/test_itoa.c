#include "itoa.h"

int main(int argc, char *argv[])
{
    char buf[0x10];
	size_t amt = itoa(buf, 0x10, 123456)
    return 0;
}
