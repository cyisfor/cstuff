#include "mmapfile.h"

#include <sys/stat.h> // fstat
#include <fcntl.h> // open
#include <sys/mman.h> // mmap
#include <assert.h>
#include <unistd.h> // close

void* mmapfd(int fd, size_t* osize) {	 
	 struct stat st;
	 if(0 != fstat(fd, &st)) {
		 close(fd);
		 return NULL;
	 }
	 *osize = st.st_size;
	 void* b = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	 if(b == MAP_FAILED) {
		 return NULL;
	 }
	 close(fd);
	 return b;
}

void* mmapfile(const char* path, size_t* osize) {
	 int fd = open(path, O_RDONLY);
	 if(fd < 0) {
		 return NULL;
	 }
	 void* ret = mmapfd(fd, osize);
	 close(fd);
	 return ret;
}

