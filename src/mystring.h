#ifndef _MYSTRING_H_
#define _MYSTRING_H_

#include <string.h> // memcpy
#include <stdio.h> // snprintf
#include <stdlib.h> // malloc
#include <stdbool.h>

#define LITSIZ(a) (sizeof(a)-1)
#define LITLEN(a) a, LITSIZ(a)

/* C makes this almost unusable. You cannot create a mutable structure that points
	 to const data. */
typedef struct cstring {
	const char* const s;
	const size_t l;
} cstring;

typedef struct string {
	const char* s;
	size_t l;
} string;

typedef struct bstring {
	char* s;
	size_t l;
	size_t space;
} bstring;

typedef struct ownablestring {
	const char* s;
	size_t l;
	bool owned; // s is owned by us.
	// note, can't be cast to/from a bstring!
} ownablestring;

#define CSTRING(str) (*((const string*)&str)) // any kind of string
#define STRING(str) (*((string*)&str)) // any kind of string, but may segfault

static
bstring bstringstr(const char* s, size_t n) {
	bstring ret = {
		.s = malloc(n),
		.l = n,
		.space = n
	};
	memcpy(ret.s,s,n);
	return ret;
}

#define bstringlit(lit) bstringstr(LITLEN(lit))

#define BLOCK_SIZE 0x200

#define strgrow(st) st.space = (st.space * 3)>>1; st.s = realloc(st.s,st.space)

#define strreserve(st,n) while(st.l + n > st.space) strgrow(st)

#define straddn(st,c,n) { strreserve(st,n); memcpy(st.s + st.l, c, n); st.l += n; }

#define stradd(st,lit) straddn(st,lit,sizeof(lit)-1)
#define straddint(st,i) strreserve(st,0x10); st.l += snprintf(st.s + st.l, 0x10, "%x",i);

#define strrewind(st) st.l = 0
#define strclear(st) free(st.s); st.s = NULL; st.l = 0; st.space = 0;

#define STRANDLEN(st) st.s, st.l


#endif /* _MYSTRING_H_ */
