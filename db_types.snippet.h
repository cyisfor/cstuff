#ifndef TYPE
#error wha
#endif

#ifdef IMPLEMENTATION
EXPORT
int CONCATSYM(db_bind_, TYPE)(db_stmt stmt, int col, BIND_ARGS) {
	return CONCATSYM(sqlite3_bind_, TYPE)(stmt->sqlite, col, BIND_PARAMS);
}
EXPORT
COLUMN_RETURN CONCATSYM(db_column_, TYPE)(db_stmt stmt, int col) {
	return CONCATSYM(sqlite3_column_, TYPE)(stmt, col);
}
#else  /* IMPLEMENTATION */
/* interface... */
int CONCATSYM(db_bind_, TYPE)(db_stmt, int col, BIND_ARGS);
COLUMN_RETURN CONCATSYM(db_column, TYPE)(db_stmt, int col);
#endif
#undef TYPE
/* leave the rest for db_all_types.snippet.h to redefine */
