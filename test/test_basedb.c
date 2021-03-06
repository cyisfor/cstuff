#include "db/base.h"

int main(int argc, char *argv[])
{
	record_init();
	basedb db = basedb_open(.path = ":memory:");
	basedb_savepoint(db);
	int res = basedb_exec(db, "CREATE TABLE IF NOT EXISTS foo (bar INTEGER)");
	ensure_eq(res, success);

	printf("has table? %s\n", basedb_has_table(db, "foo") ? "ye" : "ne");
	basedb_release(db);
	basedb_close(db);
    return 0;
}
