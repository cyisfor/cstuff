#include "db.h"
#include "mmapfile.h"
#include "mystring.h"

#include <sys/mman.h> // munmap
#include <string.h> // memchr

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <error.h>
#include <unistd.h> // sleep

struct dbpriv {
	struct db public;
	sqlite3* c;
	sqlite3_stmt *begin, *commit, *rollback;
};
typedef struct dbpriv* dbpriv;

static
db open_with_flags(const char* path, int flags) {
	dbpriv db = calloc(1,sizeof(struct dbpriv));

	//chdir(getenv("FILEDB"));
	ensure_eq(
		SQLITE_OK,
		sqlite3_open_v2(
			path, &db->c,
			flags | SQLITE_OPEN_NOMUTEX | SQLITE_OPEN_PRIVATECACHE,
			NULL));
	sqlite3_extended_result_codes(db->c, 1);
	db->begin = db_prepare((db)db, "BEGIN");
	db->commit = db_prepare((db)db, "COMMIT");
	db->rollback = db_prepare((db)db, "ROLLBACK");
	DB_OK(db);
	return (db)db;
}

db db_open(const char* path) {
	return open_with_flags(path,
						   SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE);
}

db db_read(const char* path) {
	return open_with_flags(path,
						   SQLITE_OPEN_READONLY);
}

int db_check(db db, int res)
{
	if(db->public.dberr) {
		fprintf(stderr, "check while erroring %s\n",sqlite3_errstr(res));
	}
	switch(res) {
	case SQLITE_OK:
	case SQLITE_ROW:
	case SQLITE_DONE:
		return res;
	};
	fprintf(stderr,
			"sqlite error %s (%s)\n",
			sqlite3_errstr(res), sqlite3_errmsg(db));
	fflush(stderr);
	db->public.dberr = res;
	return res;
}

void db_once(sqlite3_stmt* stmt) {
	int res = db_check(sqlite3_step(stmt));
	assert(res != SQLITE_ROW);
	sqlite3_reset(stmt);
}

bool in_transaction = false;

void db_begin() {
	if(in_transaction) {
		//db_retransaction() <- will use the innermost nested transaction, not the outermost one
		return;
	}
	in_transaction = true;
	db_once(begin);
}

void db_commit() {
	if(!in_transaction) return;
	db_once(commit);
	in_transaction = false;
}

void db_rollback() {
	if(!in_transaction) return;
	db_once(rollback);
	in_transaction = false;
}

void db_retransaction() {
	if(!in_transaction) return;
	if(db->public.dberr) {
		db_once(rollback);
		db->public.dberr = false;
	} else {
		db_once(commit);
	}
	db_once(begin);
}

void db_close(void) {
	if(in_transaction) {
		if(db->public.dberr) {
			db_once(rollback);
		} else {
			db_once(commit);
		}
	}

	sqlite3_finalize(begin);
	sqlite3_finalize(commit);
	sqlite3_finalize(rollback);

	int attempt = 0;
	for(;attempt<10;++attempt) {
		int res = sqlite3_close(c);
		if(res == SQLITE_OK) return;
		printf("sqlite close error %s %s\n", sqlite3_errstr(res), sqlite3_errmsg(c));
		if(attempt > 1) {
			sleep(attempt);
		}
		sqlite3_stmt* stmt = NULL;
		while((stmt = sqlite3_next_stmt(c, stmt))) {
			printf("closing statement\n%s\n",sqlite3_sql(stmt));
			db_check(sqlite3_finalize(stmt));
		}
	}
	error(23,23,"could not close the database");
}

static int asht(void* ctx, int cols,char** vals,char** names) {
	printf("uh %d\n",cols);
}

void db_load(const char* path, result_handler on_res) {
	size_t len = 0;
	const char* sql = mmapfile(path,&len);
	char* errmsg = NULL;
	db_execmanyn(sql, len, on_res);
	munmap((void*)sql, len);
}

int db_execn(const char* s, size_t l) {
	sqlite3_stmt* stmt = db_preparen(s,l);
	int res = sqlite3_step(stmt);
	sqlite3_finalize(stmt);
	return res;
}

result db_execmany(db public, string tail, result_handler on_res) {
	sqlite3* c = ((dbpriv)public)->c;
	db_stmt stmt = NULL;
	const char* next = NULL;
	int i = 0;
	for(;;++i) {
		string cur = {
			.base = tail.base,
			.len = 0;
		};
		int res = sqlite3_prepare_v2(priv->c,
									 tail.base, tail.len,
									 &stmt,
									 &next);
#define CHECK															\
		if(res != SQLITE_OK) {											\
			if(on_res)													\
				return on_res(res,i,stmt,cur, sql);				\
			return fail;												\
		}
		CHECK;
		if(stmt == NULL) return true; // just trailing comments, whitespace
		if(next != NULL) {
			cur.len = next - tail.base;
			tail.len -= cur.len;
			tail.base = next;
		}
		res = sqlite3_step(stmt);
		CHECK;
		res = sqlite3_finalize(stmt);
		CHECK;
		if(on_res)
			if(fail == on_res(res,i,stmt,cur,sql)) return fail;
		if(next == NULL)
			return succeed;
	}
}


sqlite3_stmt* db_prepare_str(db public, string sql) {
	sqlite3* c = ((dbpriv)public)->c;
	sqlite3_stmt* stmt = NULL;
	const char* db_next = NULL;
	int res = sqlite3_prepare_v2(
		c,
		sql.base,
		sql.len,
		&stmt,
		&db_next);
	if(db_next && db_next - sql.base != sql.len) {
		string tail = {
			.base = db_next,
			.len = sql.len - (db_next - sql.base)
		};
		record(WARNING, "some sql wouldn't prepare #.*s",
			   STRING_FOR_PRINTF(tail));
	}
	if(res != SQLITE_OK) {
		record(ERROR, "preparing %.*s",
			   STRING_FOR_PRINTF(sql));
	}
	return stmt;
}

#ifdef DEBUG
int db_stepderp(sqlite3_stmt* stmt,const char* file, int line)
{
	return db_checkderp(sqlite3_step(stmt),file, line);
}
#else
int db_step(sqlite3_stmt* stmt)
{
	int res = db_check(sqlite3_step(stmt));
	if(db->public.dberr) {
		fprintf(stderr,"stepping over %s\n",sqlite3_sql(stmt));
	}
	return res;
}
#endif

ident db_lastrow(void) {
	return sqlite3_last_insert_rowid(c);
}



bool db_has_tablen(const char* table, size_t n) {
	static sqlite3_stmt	*has_table = NULL;
	if(!has_table) {
		has_table = db_prepare(
			"SELECT name FROM sqlite_master WHERE type='table' AND name=?");
	}
	sqlite3_bind_text(has_table,1,table,n,NULL);
	// can't use db_once because we expect SQLITE_ROW
	int res = db_check(sqlite3_step(has_table));
	sqlite3_reset(has_table);
	return res == SQLITE_ROW;
}
