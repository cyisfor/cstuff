#include "myint.h"
#include "aton.h"

static
string adjust_for_zero_base(string src, int* base) {
	if(src.base[0] == '0') {
		if(src.len == 1) {
			*base = -1;
			return src;
		}
		--src.len;
		++src.base;
		switch(src.base[0]) {
		case '.':
			*base = 10;
			break;
		case 'q':
			--src.len;
			++src.base;
			assert(src.len > 0);
			*base = BASE_Q;
			break;
		case 'x':
			--src.len;
			++src.base;
			assert(src.len > 0);
			*base = 0x10;
			break;
		case 'o':
			--src.len;
			++src.base;
			assert(src.len > 0);
			*base = 010;
			break;
		default:
			*base = 010;
			break;
		};
	} else {
		*base = 10;
	}
	return src;
}

#define  FOR_BASE_Q X(Q) X(q)

static
char to_digit(char numeral, int base) {
	switch(base) {
	case 10:
		switch(numeral) {
		case '0' ... '9':
			return numeral - '0';
		default:
			return -1;
		};
	case 0x10:
		switch(numeral) {
		case '0' ... '9':
			return numeral - '0';
		case 'a' ... 'f':
			return numeral - 'a' + 10;
		case 'A' ... 'F':
			return numeral - 'A' + 10;
		default:
			return -1;
		};
	case BASE_Q:
		switch(numeral) {
		case 'Q':
		case 'q':
			return 0;
		case 'B':
		case 'b':
			return 1;
		case 'P':
		case 'p':
			return 2;
		case 'V':
		case 'v':
			return 3;
		case 'F':
		case 'f':
			return 4;
		case 'Z':
		case 'z':
			return 5;
		case 'S':
		case 's':
			return 6;
		case 'D':
		case 'd':
			return 7;
		case 'T':
		case 't':
			return 8;
		case 'J':
		case 'j':
			return 9;
		case 'C':
		case 'c':
			return 0xa;
		case 'G':
		case 'g':
			return 0xb;
		case 'K':
		case 'k':
			return 0xc;
		case 'Y':
		case 'y':
			return 0xd;
		case 'X':
		case 'x':
			return 0xe;
		case 'W':
		case 'w':
			return 0xf;
		default:
			return -1;
		};			
	case 010:
		switch(numeral) {
		case '0' ... '7':
			return numeral - '0';
		default:
			return -1;
		};
	case 2:
		switch(numeral) {
		case '0':
			return 0;
		case '1':
			return 1;
		default:
			return -1;
		};
	default: {
		int digit;
		switch(numeral) {
		case '0' ... '9':
			digit = numeral - '0';
			break;
		case 'a' ... 'z':
			digit = numeral - 'a' + 10;
			break;
		case 'A' ... 'Z':
			digit = numeral - 'A' + 10;
			break;
		default:
			return -1;
		};
		if(digit > base) {
			return -1;
		}
		return digit;
	}
	};
}

long int strtol(string src, size_t* end, int base) {
	long int ret = 0;
	size_t i;
	assert(src.len > 0);
	if(base == 0) {
		src = adjust_for_zero_base(src, &base);
		if(base == -1) return 0; /* XXX: derp */
	}
	
	for(i=0;i<src.len;++i) {
		char digit = to_digit(src.base[i], base);
		if(digit == -1) goto DONE;
		switch(base) {
		case 010:
			ret = ret << 3 | digit;
			break;
		case 0x10:
		case BASE_Q:
			ret = ret << 4 | digit;
			break;
		default:
			ret = ret * base + digit;
			break;
		};
	}
DONE:
	*end = i;
	return ret;
}

double strtod(string src, size_t* end, int base) {
	if(base == 0) {
		src = adjust_for_zero_base(src, &base);
		if(base == -1 && src.base[0] != '.') return 0; /* XXX: derp */
	}	
	const byte* dot = memchr(src.base, '.', src.len);
	/* TODO: check locale for weirdo Deutsch decimal comma */
	if(dot == NULL) {
		return strtol(src, end, base);
	}
	string intpart = {
		.base = src.base,
		.len = dot - src.base
	};
	double ret = 0;
	if(intpart.len) {
		size_t end2 = 0;
		ret = strtol(intpart, &end2, base);
		if(ret == 0 && (end2 != intpart.len)) {
			*end = end2;
			return 0.0;
		}
		/* we are at the dot. */
		*end = end2;
		assert(src.base[end2] == '.');
	}
	++*end;
	size_t i;
	double place = 1;
	int derp = base == BASE_Q ? 0x10 : base;
	for(i=intpart.len+1;i<src.len;++i) {
		char digit = to_digit(src.base[i], base);
		if(digit == -1) {
			break;
		}
		place /= derp;
		ret += digit * place;
		printf("fwhat %lf %d %lf %c\n", ret, digit, place, src.base[i]);
	}
	*end = i;
	return ret;
}
