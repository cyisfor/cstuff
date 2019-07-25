#include "aton.h"

static
string adjust_for_zero_base(int* base) {
	if(src.base[0] == '0') {
		if(src.len == 1) {
			*base = -1;
			return;
		}
		--src.len;
		++src.base;
		switch(src.base[0]) {
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
		*base = 010;
	}
}

static
int to_digit(int base, char numeral) {
		switch(base) {
		case 10:
			switch(src.base[i]) {
			case '0' ... '9':
				ret = ret * 10 + src.base[i] - '0';
				break;
			default:
				goto DONE;
			};
		break;
		case 0x10:
			switch(src.base[i]) {
			case '0' ... '9':
				ret = ret << 4 | (src.base[i] - '0');
				break;
			case 'a' ... 'f':
				ret = ret << 4 | (src.base[i] - 'a' + 10);
				break;
			case 'A' ... 'F':
				ret = ret << 4 | (src.base[i] - 'A' + 10);
				break;
			default:
				goto DONE;
			};
			break;
		case 010:
			switch(src.base[i]) {
			case '0' ... '7':
				ret = ret << 3 | (src.base[i] - '0');
				break;
			default:
				goto DONE;
			};
			break;
		case 2:
			switch(src.base[i]) {
			case '0':
				ret <<= 1;
			case '1':
				ret = ret << 1 | 1;
				break;
			default:
				goto DONE;
			};
			break;			
		default: {
			int digit;
			switch(src.base[i]) {
			case '0' ... '9':
				digit = src.base[i] - '0';
				break;
			case 'a' ... 'z':
				digit = src.base[i] - 'a' + 10;
				break;
			case 'A' ... 'Z':
				digit = src.base[i] - 'A' + 10;
				break;
			default:
				goto DONE;
			};
			if(digit > base) {
				goto DONE;
			}
			ret = ret * base + digit;
		} break;
		};	

long int strtol(string src, size_t* end, int base) {
	long int ret = 0;
	size_t i;
	assert(src.len > 0);
	if(base == 0) {
		src = adjust_for_zero_base(&base);
		if(base == -1) return 0; /* XXX: derp */
	}
	
	for(i=0;i<src.len;++i) {

	}
DONE:
	*end = i;
	return ret;
}

double strtod(string src, size_t* end, int base) {
}
