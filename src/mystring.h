#include <string.h> // memcpy
#include <stdio.h> // snprintf
#include <stdlib.h> // malloc
#include <stdbool.h>

#define LITSIZ(a) (sizeof(a)-1)
#define LITLEN(a) a, LITSIZ(a)

typedef struct string {
	const char* const s;
	const size_t l;
} string;

typedef struct mstring {
	const char* s;
	size_t l;
} mstring;

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

#define STRING(mstring) ((string)mstring) // or bstring

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
