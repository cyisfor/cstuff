#include "record.h"
#include "itoa.h"
#include "mystring.h"
#include <stdio.h>
#include <stdarg.h> // VA_*
#include <stdlib.h> // abort
#include <time.h> // 

#include <stdbool.h>

static bool show_source = false;
static bool show_timestamp = true;
static bool precise_timestamp = false;
static bool plain_log = false;
static bool abort_on_error = true;
//static bool colorize = ?

void record_init(void) {
	if(getenv("show_source")) {
		show_source = true;
	}
	if(getenv("no_timestamp")) {
		show_timestamp = false;
	} else {
		if(getenv("precise_timestamp")) {
			precise_timestamp=true;
		}
	}
	if(getenv("plain_log")) {
		plain_log = true;
	}
	if(getenv("no_abort_on_error")) {
		abort_on_error = false;
	}
}

void record_f(struct record_params p, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	if(plain_log) {
		if(show_timestamp) {
			if(precise_timestamp) {
				struct timespec when;
				if(0 != clock_gettime(CLOCK_REALTIME, &when)) abort();
				char buf[0x10];
				size_t amt = itoa(buf, 0x10, when.tv_sec);
				fwrite(buf, amt, 1, stderr);
				fputc('.', stderr);
				
			} else {
				fprintf(stderr, "%ld ", time(NULL));
			}
		}		
		if(show_source) {
			fputs(p.file, stderr);
			fputc(':',stderr);
			fprintf(stderr, "%d", p.line);
			fputc(' ', stderr);
		}
		vfprintf(stderr, fmt, args);
		va_end(args);
		return;
	}
	// eliminate common prefix between note/note.c and whatever source file it is
	size_t sourcelen = 0;
	if(show_timestamp) {
		sourcelen += fprintf(stderr, "%ld", time(NULL));
	}
	if (show_source) {
		fputc(' ',stderr);
		++sourcelen;
		const char* myfile = __FILE__;
		size_t mflen = LITSIZ(__FILE__);
		size_t i;
		if(mflen > p.flen) {
			mflen = p.flen;
		}
		for(i=0;i<mflen;++i) {					
			if(p.file[i] != myfile[i]) break;
		}
		if(i != p.flen) {
			fwrite(p.file+i, p.flen-i, 1, stderr);
			sourcelen += p.flen-i;
		}
		fputc(':',stderr);
		++sourcelen;
		sourcelen += fprintf(stderr, "%d", p.line);
		fputc(' ', stderr);
		++sourcelen;
	}
	static size_t maxsourcelen = 0;
	/* align log right of source */
	if(maxsourcelen < sourcelen) {
		maxsourcelen = sourcelen;
	} else {
		int i;
		for(i=0;i<maxsourcelen-sourcelen;++i) {
			fputc(' ', stderr);
		}
	}
	fputc(' ', stderr);

	switch(p.level) {
	case ERROR:
		fwrite(LITLEN("ERROR "), 1, stderr);
		break;
	case WARNING:
		fwrite(LITLEN("WARNING "), 1, stderr);
		break;
	case INFO:
		fwrite(LITLEN("INFO "), 1, stderr);
		break;
	case DEBUG:
		fwrite(LITLEN("DEBUG "), 1, stderr);
		break;		
	};
	vfprintf(stderr, fmt, args);
	va_end(args);
	fputc('\n',stderr);
	if(abort_on_error && p.level == ERROR) {
		abort();
	}
}
