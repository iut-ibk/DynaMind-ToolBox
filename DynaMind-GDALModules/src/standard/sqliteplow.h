#ifndef SQLITEPLOW_H
#define SQLITEPLOW_H

#include <sqlite3.h>
#include <QObject>
#include <QMutex>


class SqlitePlow : public QObject
{
	Q_OBJECT
private:
	sqlite3 *db;
	std::vector<std::string> results_vector;

	std::vector<std::vector<std::string> > chunk_results;
	QMutex mMutex;
	bool stillMore;

	std::string workerQuery;
	std::string database_file;
	std::string main_table;

	void execute_query(sqlite3 *db, const char *sql, bool cb = false);

	int chunk_counter;
	int done_counter;
	int junk_size;

public:
	SqlitePlow(QObject *parent = 0);
	void commit_result(std::string uq);
	void setStillMore(bool v);
	void setWorkerQuery(std::string query);
	std::string GetWorkerQuery();
	void setMainTable(std::string main_table);
	std::string GetMainTable();
	int getTotalChunks();
	void setJunkSize(int size);

	void setDatabaseFile(std::string database_file);
	std::string getDatabaseFile();

	static int callback(void *plow, int argc, char **argv, char **azColName);
	void register_results(std::vector<std::string> vr);

public slots:
	void plow();

signals:
	void finished();
};



#endif // SQLITEWORKHORSE_H
