#include "mystring.h"
#include <pcre.h>

enum pat_mode {
    pat_plain,
    pat_pcre,
};

struct pat;

struct pat_plain_info {
	bool caseless;
	bool match_first;
};

struct pat* pat_pcre_compile(string pattern);
struct pat* pat_plain_compile(string pattern, struct pat_plain_info info);
void pat_cleanup(struct pat**);

bool pat_check(struct pat*, string test);

struct pat_captures {
	bool matched;
	int* ovector;
	int ovecsize;
};

struct pat_captures pat_capture(struct pat* parent, string test, int start);
void pat_capture_done(struct pat_captures* cap);

void pats_init(void);
void pats_uninit(void);
