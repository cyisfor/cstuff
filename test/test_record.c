#include "record.h"

int main(int argc, char *argv[])
{
	record_init();
    record(WARNING, "This is a warning %d", 42);
	record(INFO, "This is a info %d", 42);
	record(ERROR, "This is a error %d", 23);
	record(WARNING, "This never happens %d", 23);
    return 0;
}
