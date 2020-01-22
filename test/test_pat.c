#include "pat.h"

int main(int argc, char *argv[])
{
	pats_init();
    struct pat* plain = pat_plain_compile(LITSTR("ferrets"), (struct pat_plain_info){
			.caseless = true,
				.match_first = false
				});

	const string ferrets = LITSTR("Have some ferrets have some more ferrets ferrets ferrets ferrets");
	struct pat_captures cap = pat_capture(plain, ferrets, 0);
	size_t i = 0;
	for(;i<cap.ovecsize;++i) {
		printf("capture %ld %d %.*s",
			   i, cap.ovector[i],
			   (int)LITSIZ("ferrets"),
			   ferrets.base + cap.ovector[i]);
	}

	pat_capture_done(&cap);
	pat_cleanup(&plain);
	pats_uninit();
		
    return 0;
}
