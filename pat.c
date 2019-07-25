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
};

static void pats_init(void) {
    stack = pcre_jit_stack_alloc(0x100,0x80000);
}

void pats_uninit(void) {
    if(stack) {
        pcre_jit_stack_free(stack);
        stack = NULL;
    }
}

struct pat* pat_setup(const string pattern, enum pat_mode mode) {
    const char* err = NULL;
    int erroffset = 0;

    if(mode == pat_plain || mode == pat_match) {
        struct plain_pat* self = g_new(struct plain_pat,1);
        self->parent.mode = mode;
        self->caseless = (mode == pat_match) ? TRUE : FALSE;
        if(mode == pat_match) {
            self->caseless = TRUE;
            // needs freeing
            self->substring = {
				.base = g_ascii_strdown(pattern.base, pattern.len),
				.len = pattern.len
			};
        } else {
            self->caseless = FALSE;
            self->substring = pattern; // assuming this is a string literal
        }
        return (struct pat*)self;
    } else {
        struct pcre_pat* self = g_new(struct pcre_pat,1);
        assert(self);
        self->parent.mode = mode;
        self->pat = pcre_compile(pattern,0,
                &err,&erroffset,NULL);
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
}

void pat_cleanup(struct pat** self) {
    struct pat* doomed = *self;
    *self = NULL;

    if(doomed->mode == pat_plain || doomed->mode == pat_match) {
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

    g_free(doomed);
}

gboolean pat_check(struct pat* parent, string test) {
    if(parent->mode == pat_plain || parent->mode == pat_match) {
        struct plain_pat* self = (struct plain_pat*) parent;
		if(test.len != self->substring.len) return false;
        if(self->caseless==TRUE) 
            test.base = g_ascii_strdown(test.base,test.len);
        const char* found = memmem(
			test.base, test.len,
			self->substring.base,
			self->substring.len);
        if(self->caseless==TRUE)
            g_free((char*)test);
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

  if(rc >= 0) {
      return TRUE;
  }
  return FALSE;
}

struct pat_captures pat_capture(struct pat* parent, string test, int start) {
    g_assert(parent->mode == pat_pcre);
    struct pcre_pat* self = (struct pcre_pat*) parent;
	if(self->ovecsize == 0) {
		int res = pcre_fullinfo(self->pat,
								self->study,
								PCRE_INFO_CAPTURECOUNT,
								&self->ovecsize);
		g_assert(res == 0);
		self->ovecsize = self->ovecsize * 3 / 2 + 2;
	}
    int rc = pcre_jit_exec(
		self->pat,
		self->study,
		test.base,
		test.len,
		start,
		0,
		ovector,
		ovecsize,
		stack);
	return rc >= 0;
}

