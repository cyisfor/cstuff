// CPP sux
#define symjoin(a,b) a ## b

#define DEFER() void symjoin(cleanupfunc,__LINE__)(void* nada); \
	char __attribute__((__cleanup__(symjoin(cleanupfunc,__LINE__)))) \
	void symjoin(cleanupfunc,__LINE__)(void* nada)
