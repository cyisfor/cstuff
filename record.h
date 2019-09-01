#ifndef RECORD_H
#define RECORD_H

enum level {
	ERROR,
	WARNING,
	INFO,
	DEBUG
};

void record_init(void); 		/* TODO: customize this w/out getenv? */

struct record_params {
	enum level level;
	const char* file;
	int flen;
	int line;
};

void record_f(struct record_params, const char* fmt, ...);
#define record(level, fmt, ...) ({					\
			struct record_params params = {			\
				level,								\
				__FILE__,							\
				sizeof(__FILE__)-1,					\
				__LINE__							\
			};										\
			record_f(params, fmt, ## __VA_ARGS__);	\
		})

#endif /* RECORD_H */
