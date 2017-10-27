#include "mmapfile.h"

#include <sys/stat.h> // fstat
#include <fcntl.h> // open
#include <sys/mman.h> // mmap
#include <assert.h>
#include <unistd.h> // close

void mmapfd(int fd, size_t* osize) {	 
	 struct stat st;
	 assert(0 == fstat(fd, &st));
	 void* b = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	 assert(b != MAP_FAILED);
	 close(fd);
	 *osize = st.st_size;
	 return b;
}

void* mmapfile(const char* path, size_t* osize) {
	 int fd = open(path, O_RDONLY);
	 assert(fd >= 0);
	 void* ret = mmapfd(fd, osize);
	 close(fd);
	 return ret;
}

