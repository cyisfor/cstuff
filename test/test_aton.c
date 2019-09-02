#include "aton.h"
#include "mystring.h"
int main(int argc, char *argv[])
{
	size_t end = 0;
	long int answer = strtol(LITSTR("42"), &end, 10);
	if(end != 2) return 1;
	if(answer != 42) return 2;
	answer = strtol(LITSTR("42"), &end, 13);
	if(end != 2) return 3;
	if(answer != 9*6) return 4;

	answer = strtol(LITSTR("BD"), &end, BASE_Q);
	if(end != 2) return 5;
	if(answer != 23) return 6;
    
    return 0;
}
