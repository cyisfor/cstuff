#include "db/base.h"

int main(int argc, char *argv[])
{
	record_init();
	basedb db = basedb_open(.path = "/tmp/derp.sqlite");
	basedb_savepoint(db);
	int res = basedb_exec(db, "CREATE TABLE foo (bar INTEGER)");
	ensure_eq(res, succeed);

	printf("has table? %s\n", basedb_has_table(db, "foo") ? "ye" : "ne");
	basedb_release(db);
	basedb_close(db);
    return 0;
}
