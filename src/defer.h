#include "symjoin.h"
#define DEFERNAME(name,var) auto void name(void* nada); \
	char __attribute__((__cleanup__(name))) var; \
	void name(void* nada)

#define DEFER DEFERNAME(symjoin(cleanupfunc,__LINE__),symjoin(defervar,__LINE__))
