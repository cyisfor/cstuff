#ifndef RESULT_H
#define RESULT_H

#ifndef T
#define T result
#define N(a) result_ ## a
#endif

typedef enum T {
	N(failure),
	N(success),
	N(done),
	N(undefined),
	N(pending),
	N(unknown),
	N(ambiguous),
	N(busy),
} result;

#endif /* RESULT_H */
