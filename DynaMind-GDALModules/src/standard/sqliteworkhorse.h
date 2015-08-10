#ifndef SQLITEWORKHORSE_H
#define SQLITEWORKHORSE_H

#include "sqliteplow.h"

#include <vector>

#include <sqlite3.h>

#include <QRunnable>
#include <QObject>

class SqliteWorkHorse : public QObject, public QRunnable
{
	Q_OBJECT
private:
	sqlite3 *db;
	long id;
	long start;
	long end;
	void initDatabase();
	SqlitePlow * plow;
	std::vector<std::string> result_vector;
	std::string main_table;

public:
	std::string getMainTable();

	SqliteWorkHorse();
	virtual ~SqliteWorkHorse();
	void register_result(std::string r);

	SqliteWorkHorse(SqlitePlow * plow, long id,long start, long end);
	void run();

	void execute_query(const char *sql, bool with_callback = false);
	static int callback(void *db_worker, int argc, char **argv, char **azColName);

};

#endif // SQLITEWORKHORSE_H
