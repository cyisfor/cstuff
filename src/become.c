#include "become.h"

struct becomer* become_start(const char* destname) {
	struct becomer* b = malloc(sizeof(struct becomer));
	b->got_times = false;
	b->destname = destname;
	/* don't bother saving this, since the kernel is retarded about
		 strings. */
	size_t destlen = strlen(destname);
	b->tempname = malloc(destlen + 5);
	memcpy(b->tempname, destname, destlen);
	memcpy(b->tempname+destlen,".tmp\0",5);
	b->out = fopen(b->tempname,"wb");
	return b;
}
void become_abort(struct becomer** bb) {
	struct becomer* b = *bb;
	*bb = NULL;
	unlink(b->tempname);
	fclose(b->out);
	free(b->tempname);
	free(b);
}
void become_commit(struct becomer** bb) {
	struct becomer* b = *bb;
	*bb = NULL;
	fflush(b->out);
	if(0 != fsync(fileno(b->out))) abort();
	if(0 != rename(b->tempname, b->destname)) abort();
	free(b->tempname);
	if(b->got_modified) {
		futimens(fileno(b->out), b->times);
	}
	fclose(b->out);
	free(b);
}
	
	
