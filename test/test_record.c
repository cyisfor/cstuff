#include "record.h"
#include <unistd.h> // fork
#include <sys/wait.h> // waitpid
#include <stdio.h> // 


int main(int argc, char *argv[])
{
	int pid = fork();
	if(pid == 0) {
		record_init();
		record(WARNING, "This is a warning %d", 42);
		record(INFO, "This is a info %d", 42);
		record(ERROR, "This is a error %d", 23);
		record(WARNING, "This never happens %d", 23);
	}
	int status;
	if(waitpid(pid, &status,0) != pid) return 1;
	if(!WIFEXITED(status)) {
		int sig = WTERMSIG(status);
		if(sig == SIGABRT) {
			return 0;
		}
		return -sig;
	}
	return 1 + WEXITSTATUS(status);
}
