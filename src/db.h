#include <sqlite3.h>
#include <stdlib.h> // size_t
#include <stdbool.h>

typedef sqlite3_int64 ident;

sqlite3* db_init(void);
void db_begin(void);
void db_commit(void);
void db_retransaction(void);
void db_close(void);

#ifdef DEBUG
#define db_check(res) db_checkderp(res,__FILE__,__LINE__)
int db_checkderp(int res, const char* file, int line);

#define db_step(stmt) db_stepderp(stmt,__FILE__,__LINE__)
int db_stepderp(sqlite3_stmt* stmt, const char* file, int line);
#else
int db_check(int res);
int db_step(sqlite3_stmt* stmt);
#endif

#define db_exec(st) db_execn(st.s,st.l)
int db_execn(const char* s, size_t l);

typedef bool (*result_handler)(int res, int n, sqlite3_stmt* stmt, const char* tail, size_t sl, size_t l);

extern result_handler default_result_handler;

#define db_execmany(st,on_err) db_execmanyn(st.s,st.l,on_err)
void db_execmanyn(const char* s, size_t l, result_handler on_err);

void db_load(const char* path, result_handler on_res);

#define db_prepare(lit) db_preparen(lit,sizeof(lit)-1);
sqlite3_stmt* db_preparen(const char* s, size_t l);

ident db_lastrow(void);
