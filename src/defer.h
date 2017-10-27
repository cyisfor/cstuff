// CPP sux
#define symjoin2(a,b) a ## b
#define symjoin(a,b) symjoin2(a,b)

#define DEFER auto void symjoin(cleanupfunc,__LINE__)(void* nada); \
	char __attribute__((__cleanup__(symjoin(cleanupfunc,__LINE__)))) symjoin(defervar,__LINE__); \
	void symjoin(cleanupfunc,__LINE__)(void* nada)
