#include <sqlite3.h>
#include <stdlib.h> // size_t
#include <stdbool.h>

typedef sqlite3_int64 ident;

void db_init(const char* path);
void db_close(void);

// this is just to be easier to read...
#define BIND(type,stmt,column,...) sqlite3_bind_ ## type(stmt, column, ## __VA_ARGS__)

#ifdef DEBUG
#define db_check(res) db_checkderp(res,__FILE__,__LINE__)
int db_checkderp(int res, const char* file, int line);

#define db_step(stmt) db_stepderp(stmt,__FILE__,__LINE__)
int db_stepderp(sqlite3_stmt* stmt, const char* file, int line);
#else
int db_check(int res);
int db_step(sqlite3_stmt* stmt);
#endif

extern int dberr;

#define DB_OK if(dberr != 0) abort();

void db_once(sqlite3_stmt* stmt);

#define db_exec(st) db_execn(st.s,st.l)
int db_execn(const char* s, size_t l);

#define RESULT_HANDLER(name) \
	bool name(int res, int n, sqlite3_stmt* stmt, const char* tail, size_t sl, size_t l)

typedef RESULT_HANDLER((*result_handler));

extern result_handler default_result_handler;

#define db_execmany(st,on_err) db_execmanyn(st.s,st.l,on_err)
void db_execmanyn(const char* s, size_t l, result_handler on_err);

void db_load(const char* path, result_handler on_res);

extern const char* db_next; // ehhh
#define db_prepare(lit) db_preparen(lit,sizeof(lit)-1);
sqlite3_stmt* db_preparen(const char* s, size_t l);

ident db_lastrow(void);

void db_begin(void);
void db_commit(void);
void db_retransaction(void);

#include "defer.h"

#define TRANSACTION db_begin(); DEFER { if(dberr) db_rollback() else db_commit(); }


bool db_has_tablen(const char* table, size_t n);
#define db_has_table(lit) db_has_tablen(LITLEN(lit))
