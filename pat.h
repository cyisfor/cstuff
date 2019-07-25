#include "mystring.h"
#include <pcre.h>

enum pat_mode {
    pat_plain,
    pat_pcre,
    pat_match
};

struct pat;

struct pat* pat_setup(string pattern, enum pat_mode mode);
void pat_cleanup(struct pat**);

bool pat_check(struct pat*, const char* test);

struct pat_captures {
	bool matched;
	int* ovector;
	int ovecsize;
};

struct pat_captures pat_capture(struct pat* parent, string test, int start);

void pat_capture_done(struct pat_captures);

void pats_uninit(void);
