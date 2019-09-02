#include "db/base.h"

int main(int argc, char *argv[])
{
	basedb db = basedb_open(.path = ":memory:");
	int res = basedb_exec(db, "CREATE TABLE foo (bar INTEGER)");
	ensure_eq(res, succeed);

	printf("has table? %s\n", basedb_table(db, "foo") ? "ye" : "ne");
	
    return 0;
}
