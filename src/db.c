#include <sqlite3.h>

sqlite3* db = NULL;
void db_init(void) {
	chdir(getenv("HOME"));
	chdir(".local"); // don't bother check if success, just put it in home
 
	assert(0 == sqlite3_open("pics.sqlite",&db));
	chdir("/");
	assert(db != NULL);
	sqlite3_busy_timeout(db, 10000);
	char* errmsg = NULL;
	int res = sqlite3_exec(db, base_sql, NULL, NULL, &errmsg);
	if(res != 0) {
		error(1,res,errmsg);
	}

	
