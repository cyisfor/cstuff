#include "myint.h"
#include "aton.h"

static
void adjust_for_zero_base(string src, int* base, int* head) {
	*head = 0;
	if(src.base[0] == 'Q') {
		if(src.len == 1) {
			*base = -1;
			return;
		}
		switch(src.base[1]) {
		case 'Q':
		case 'B':
		case 'P':
		case 'V':
		case 'F':
		case 'Z':
		case 'S':
		case 'D':
		case 'T':
		case 'J':
		case 'C':
		case 'G':
		case 'K':
		case 'Y':
		case 'X':
		case 'W':
			*base = BASE_Q;
			return;
		default:
			*base = -1;
			return;
		};
	}

		
	if(src.base[0] == '0') {
		if(src.len == 1) {
			*base = -1;
			return;
		}
		++*head;
		switch(src.base[*head]) {
		case '.':
			*base = 10;
			break;
		case 'q':
			++*head;
			assert(src.len > *head);
			*base = BASE_Q;
			break;
		case 'x':
			++*head;
			assert(src.len > 0);
			*base = 0x10;
			break;
		case 'o':
			++*head;
			assert(src.len > *head);
			*base = 010;
			break;
		default:
			*base = 010;
			break;
		};
	} else {
		*base = 10;
	}
	return;
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
			return 0;
		case 'B':
			return 1;
		case 'P':
			return 2;
		case 'V':
			return 3;
		case 'F':
			return 4;
		case 'Z':
			return 5;
		case 'S':
			return 6;
		case 'D':
			return 7;
		case 'T':
			return 8;
		case 'J':
			return 9;
		case 'C':
			return 0xa;
		case 'G':
			return 0xb;
		case 'K':
			return 0xc;
		case 'Y':
			return 0xd;
		case 'X':
			return 0xe;
		case 'W':
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
	int head = 0;
	if(base == 0) {
		adjust_for_zero_base(src, &base, &head);
		if(base == -1) return 0; /* XXX: derp */
	}
	
	for(i=head;i<src.len;++i) {
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
	int head = 0;
	const byte* dot = NULL;
	if(base == 0) {
		adjust_for_zero_base(src, &base, &head);
		if(base == -1) {
			if(src.base[0] == '.') {
				dot = 0;
				string derp = {
					src.base+1,
					src.len-1
				};
				int derphead;
				adjust_for_zero_base(derp, &base, &derphead);
				if(base == -1)
					return 0;
				head = 0;
			} else {
				return 0; /* XXX: derp */
			}
		}
	}
	
	if(dot == NULL) {
		dot = memchr(src.base, '.', src.len);
		/* TODO: check locale for weirdo Deutsch decimal comma */
		if(dot == NULL) {
			return strtol(src, end, base);
		}
	}
	string intpart = {
		.base = src.base + head,
		.len = dot - src.base - head
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
		assert(src.base[end2] == '.');
	}
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
	}
	*end = i;
	return ret;
}
