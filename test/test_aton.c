#include "aton.h"
int main(int argc, char *argv[])
{
	size_t end = 0;
	long int res = strtol(LITSTR("42"), &end, 10);
	printf("%ld %ld\n", res, end);
    
    return 0;
}
