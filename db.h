#include "result.h"
#include "mystring.h"

#include <sqlite3.h>
#include <stdlib.h> // size_t
#include <stdbool.h>

typedef sqlite3_int64 ident;

struct db {
	int dberr;
};
typedef struct db* db;

struct db_open_params {
	const char* path;
	bool readonly;
};
db db_open_f(struct db_open_params);
#define db_open(...) ({									\
		struct db_open_params params = {__VA_ARGS__};	\
		db_open_f(params);								\
	})
void db_close(db db);
size_t db_stmt_changes(db db);

typedef struct db_stmt *db_stmt;

#define db_prepare(lit) db_prepare_str(LITSTR(lit));
db_stmt db_prepare_str(string sql);
void db_reset(db_stmt stmt);
void db_finalize(db_stmt stmt);
void db_once(db_stmt stmt);
int db_step(db_stmt stmt);
size_t db_stmt_changes(db_stmt stmt);

static int db_change(db_stmt stmt) {
/* insert, update or delete */
	ensure_eq(SQLITE_DONE, db_step(stmt));
	return db_stmt_changes(stmt);
}

#define TYPE blob
#define BIND_ARGS const void* blob, int len, void(*)(void*)destructor
#define COLUMN_RETURN const void*
#include "db_types.snippet.h"

int db_check(int res);

#define db_exec(lit) db_exec_str(LITSTR(lit))
int db_exec_str(string sql);

#define RESULT_HANDLER(name) \
	bool name(int res, int n, sqlite3_stmt* stmt, string sql, string tail)

typedef RESULT_HANDLER((*result_handler));

extern result_handler default_result_handler;

void db_execmany(string sql, result_handler on_err);

void db_load(const char* path, result_handler on_res);

extern const char* db_next; // ehhh

ident db_lastrow(void);

void db_begin(void);
void db_commit(void);
void db_retransaction(void);

#include "defer.h"

#define TRANSACTION db_begin(); DEFER { if(dberr) db_rollback() else db_commit(); }


bool db_has_table_str(string);
#define db_has_table(lit) db_has_table_str(LITSTR(lit))
