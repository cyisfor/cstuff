#define _GNU_SOURCE 			/* memmem */
#include "pat.h"

#include <glib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

__thread pcre_jit_stack* stack = NULL;

struct pat {
    enum pat_mode mode;
};

struct pcre_pat {
    struct pat parent; 
    pcre* pat;
    pcre_extra* study;
	int ovecsize;
};

struct plain_pat {
    struct pat parent;
    string substring;
    gboolean caseless;
	gboolean match_first;
};

void pats_init(void) {
    stack = pcre_jit_stack_alloc(0x100,0x80000);
}

void pats_uninit(void) {
    if(stack) {
        pcre_jit_stack_free(stack);
        stack = NULL;
    }
}

struct pat* pat_plain_compile(const string pattern, const struct pat_plain_info info) {
	struct plain_pat* self = g_slice_new(struct plain_pat);
	self->parent.mode = pat_plain;
	self->caseless = info.caseless;
	self->match_first = info.match_first;
	if(self->caseless) {
		// needs freeing
		self->substring = (string){
			.base = g_ascii_strdown(pattern.base, pattern.len),
			.len = pattern.len
		};
	} else {
		self->substring = pattern; // assuming this is a string literal
	}
	return (struct pat*)self;
}

struct pat* pat_pcre_compile(const string pattern) {
    const char* err = NULL;
    int erroffset = 0;
	    
	struct pcre_pat* self = g_slice_new0(struct pcre_pat);
	assert(self);
	const char* zzz;
	if(pattern.base[pattern.len-1] != '\0') {
		// sigh
		zzz = ZSTR(pattern);
	} else {
		zzz = pattern.base;
	}
	self->parent.mode = pat_pcre;
	self->pat = pcre_compile(zzz, 0,
							 &err,&erroffset,NULL);
	ZSTR_done();
	if(!self->pat) {
		fprintf(stderr,"PCRE COMPILE ERROR %s\n",err);
		abort();
		return NULL;
	}

	self->study = pcre_study(self->pat,PCRE_STUDY_JIT_COMPILE,&err);
	if(err) {
		fprintf(stderr,"Eh, study failed. %s\n",err);
		assert(self->study==NULL);
	}
	return (struct pat*)self;
}

void pat_cleanup(struct pat** self) {
    struct pat* doomed = *self;
    *self = NULL;

    if(doomed->mode == pat_plain) {
        struct plain_pat* cdoom = (struct plain_pat*) doomed;
        if(cdoom->caseless)
            g_free((char*)cdoom->substring.base);
    } else {
        struct pcre_pat* pdoom = (struct pcre_pat*) doomed;
        if(pdoom->pat) 
            pcre_free(pdoom->pat);
        if(pdoom->study)
            pcre_free_study(pdoom->study);
    }

    g_slice_free(struct pat*, doomed);
}

bool pat_check(struct pat* parent, const string test) {
    if(parent->mode == pat_plain) {
        struct plain_pat* self = (struct plain_pat*) parent;
		if(test.len != self->substring.len) return false;
		string test2 = test;
        if(self->caseless==TRUE) 
            test2.base = g_ascii_strdown(test.base,test.len);
        const char* found = memmem(
			test2.base, test2.len,
			self->substring.base,
			self->substring.len);
        if(self->caseless==TRUE)
            g_free((char*)test2.base);
		return found != NULL;
    }
    struct pcre_pat* self = (struct pcre_pat*) parent;
    // doing this check just to be safe eh
    if(!stack) pats_init();
    int rc = pcre_jit_exec(
		self->pat,                   /* the compiled pattern */
		self->study,             /* no extra data - we didn't study the pattern */
		test.base,              /* the subject string */
		test.len,       /* the length of the subject */
		0,                    /* start at offset 0 in the subject */
		0,                    /* default options */
		NULL,              /* output vector for substring information */
		0,           /* number of elements in the output vector */
		stack); /* jit stack */

	return rc >= 0;
}

static
struct pat_captures pcre_pat_capture(struct pat* parent, const string test, int start) {
    g_assert(parent->mode == pat_pcre);
    struct pcre_pat* self = (struct pcre_pat*) parent;
	if(self->ovecsize == 0) {
		int captures;
		int res = pcre_fullinfo(self->pat,
								self->study,
								PCRE_INFO_CAPTURECOUNT,
								&captures);
		g_assert(res == 0);
		/* 2 captures = 3, since captures whole pattern
		   3 captures = 0,0,1,1,2,2 (6 slots)
		   but needs 1/3 for workspace
		   (a+n)*2/3 >= a
		   a + n = a * 3 / 2
		   n = a * 3 / 2 - a
		   n = a * 1/2
		   a + n = a * 3/2
		   sizetotal = a+n = a * 3/2
		   a = 2 * (captures+1)
		   b = a * 3/2

		   So for 2 captures, a = 6, b = 9
		   for 1 capture, a = 4, b = 6
		   for 3 captures, a = 8, b = 12
		*/
		int a = 2 * (captures+1);
		int b = a * 3;
		if(b % 2 == 1) {
			self->ovecsize = b / 2 + 1;
		} else {
			self->ovecsize = b / 2;
		}
	}
	struct pat_captures cap = {
		.ovector = g_slice_alloc(self->ovecsize * sizeof(int)),
		.ovecsize = self->ovecsize
	};
    int rc = pcre_jit_exec(
		self->pat,
		self->study,
		test.base,
		test.len,
		start,
		0,
		cap.ovector,
		cap.ovecsize,
		stack);
	cap.matched = rc >= 0;
	return cap;
}

static
struct pat_captures plain_pat_capture(struct pat* parent, const string test, int start) {
	struct plain_pat* self = (struct plain_pat*)parent;
	const byte* cur = test.base + start;
	struct pat_captures cap = {};
	size_t captures = 0;
	for(;;) {
		const byte* next = memmem(
			cur, test.len - (cur - test.base),
			self->substring.base, self->substring.len);
		if(next == NULL) break;
		++captures;
		if(self->match_first) {
			/* if you want to know the bounds only of the first match */
			break;
		}
		cur = next + self->substring.len;
	}
	cur = test.base + start;
	cap.ovecsize = captures;
	cap.ovector = g_slice_alloc(cap.ovecsize * sizeof(*cap.ovector));
	captures = 0;
	for(;;) {
		const byte* next = memmem(
			cur, test.len - (cur - test.base),
			self->substring.base, self->substring.len);
		if(next == NULL) break;
		cap.ovector[captures] = next - test.base;
		++captures;
		if(self->match_first) {
			break;
		}
		cur = next + self->substring.len;
	}

	return cap;
}

struct pat_captures pat_capture(struct pat* parent, const string test, int start) {
	switch(parent->mode) {
	case pat_plain:
		return plain_pat_capture(parent, test, start);
	case pat_pcre:
		return pcre_pat_capture(parent, test, start);
	};
}

void pat_capture_done(struct pat_captures* cap) {
	if(cap->ovecsize) {
		g_slice_free1(cap->ovecsize * sizeof(int), cap->ovector);
	}
	cap->ovector = NULL;
	cap->ovecsize = 0;
}
