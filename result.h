#ifndef RESULT_H
#define RESULT_H

typedef enum result {
	failure,
	success,
	undefined,
	result_pending,
	result_unknown,
	result_ambiguous,
} result;

#endif /* RESULT_H */
