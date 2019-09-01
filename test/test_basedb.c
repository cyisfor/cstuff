#include "basedb.h"

int main(int argc, char *argv[])
{
	basedb db = basedb_open(":memory:");
	int res = basedb_exec(db, "CREATE TABLE foo (bar INTEGER)");
	ensure_eq(res, BASEDB_OK);
	
    return 0;
}
