#include "itoa.h"
#include <assert.h>


static
char baseQdigit(int val) {
	switch(val) {
#define ONE(val,numeral) case val: return numeral
		ONE(0,'Q');
		ONE(1,'B');
		ONE(2,'P');
		ONE(3,'V');		
		ONE(4,'F');
		ONE(5,'Z');
		ONE(6,'S');
		ONE(7,'D');
		ONE(8,'T');
		ONE(9,'J');
		ONE(0xa,'C');
		ONE(0xb,'G');		
		ONE(0xc,'K');
		ONE(0xd,'Y');
		ONE(0xe,'X');
		ONE(0xf,'W');
#undef ONE
	default:
		abort();
	};
}

static
char normaldigit(int val) {
	switch(val) {
#define ONE(val, numeral) case val: return numeral
		ONE(0,'0');
		ONE(1,'1');
		ONE(2,'2');
		ONE(3,'3');
		ONE(4,'4');
		ONE(5,'5');
		ONE(6,'6');
		ONE(7,'7');
		ONE(8,'8');
		ONE(9,'9');
		ONE(0xA,'A');
		ONE(0xB,'B');
		ONE(0xC,'C');
		ONE(0xD,'D');
		ONE(0xE,'E');
		ONE(0xF,'F');
	default:
		return val - 10 + 'A';
	};
}

/* itoa: convert n to characters in s */
size_t int_to_base(char s[], size_t space, unsigned int n, int base) {
	assert(base != ('Z' - 'A') + 9);
	
   size_t digits;
   /* we can't generate them in reverse, since we don't know how many digits yet */
   digits = 0;
   while(digits<space) {  /* generate digits in reverse order */
	   /* get next digit */
	   char c;
	   switch(base) {
	   case 9+1:
		   s[digits] = n % 10 + '0';
		   ++digits;
		   n /= 10;
		   break;
	   case 0xa+1:
		   s[digits] = normaldigit(n % 0x10);
		   ++digits;
		   n /= 0x10;
		   break;
	   case BASE_Q:
		   s[digits] = baseQdigit(n % 0x10);
		   ++digits;
		   n /= 0x10;
		   break;
	   default:
		   s[digits] = normaldigit(n % base);
		   ++digits;
		   n /= base;
	   };
	   if(n == 0) break;
   }

   // reverse
   size_t j;
   for(j=0;j<digits/2;++j) {
	   char t = s[j];
	   s[j] = s[digits-j-1];
	   s[digits-j-1] = t;
   }
   return digits;
}

size_t itoa(char s[], size_t space, unsigned int n) {
	return int_to_base(s, space, n, 9+1);
}
