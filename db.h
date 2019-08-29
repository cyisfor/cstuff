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
#define db_open(...) { struct db_open_params params = {...}; db_open_f(params); }
void db_close(db);

#define db_prepare(lit) db_prepare_str(LITSTR(lit));
sqlite3_stmt* db_prepare_str(string sql);


// this is just to be easier to read...
#define DB_BIND(type,stmt,column,...) sqlite3_bind_ ## type(stmt, column, ## __VA_ARGS__)

int db_check(int res);
int db_step(sqlite3_stmt* stmt);

#define DB_OK(db) if(db->dberr != 0) abort();

void db_once(sqlite3_stmt* stmt);

/* insert, update or delete */
static
int db_change(sqlite3_stmt* stmt);

#define db_exec(st) db_execn(st.s,st.l)
int db_execn(const char* s, size_t l);

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


bool db_has_tablen(const char* table, size_t n);
#define db_has_table(lit) db_has_tablen(LITLEN(lit))
