#include <stdlib.h> // size_t
void* mmapfd(int fd, size_t* osize);
void* mmapfile(const char* path, size_t* osize);
