#include <time.h> // struct timespec
#include <stdbool.h>
#include <stdio.h> // FILE*

struct becomer {
	FILE* out;
	const char* destname;
	char* tempname;
	bool got_times;
	struct timespec times[2];
};

struct becomer* become_start(const char* destname);
void become_abort(struct becomer**);
void become_commit(struct becomer**);
