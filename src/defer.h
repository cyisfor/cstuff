// CPP sux
#define symjoin2(a,b) a ## b
#define symjoin(a,b) symjoin2(a,b)

#define DEFERNAME(name,var) auto void name(void* nada); \
	char __attribute__((__cleanup__(name))) var; \
	void name(void* nada)

#define DEFER DEFERNAME(symjoin(cleanupfunc,__LINE__),symjoin(defervar,__LINE__))
