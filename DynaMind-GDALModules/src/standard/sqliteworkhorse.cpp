#include "sqliteworkhorse.h"

#include <iostream>
#include <sstream>
#include <dmlogger.h>

SqliteWorkHorse::SqliteWorkHorse()
{
}

int SqliteWorkHorse::callback(void *db_w, int argc, char **argv, char **azColName){
	if (argc == 0)
		return 0;

	SqliteWorkHorse* db_worker = reinterpret_cast<SqliteWorkHorse*>(db_w);

	std::vector<std::string> values(argv , argv + argc);
	std::vector<std::string> names(azColName, azColName + argc);

	std::stringstream query;
	query << "UPDATE ";
	query << db_worker->getMainTable();
	query << " SET ";

	for (int i = 1; i < values.size(); i++) {
		if (i > 2)
			query << ",";
		query << names[i]<< " = " << values[i];
	}

	query  << " WHERE ogc_fid = " << values[0];


	db_worker->register_result(query.str());

	return 0;
}


SqliteWorkHorse::SqliteWorkHorse(SqlitePlow * plow, long id, long start, long end): plow(plow), id(id),start(start), end(end)
{

}

void SqliteWorkHorse::execute_query(const char *sql, bool with_callback) {
	char *zErrMsg = 0;

	int rc;
	if (with_callback) {
		rc = sqlite3_exec(db, sql , callback, this, &zErrMsg);
	}
	else
		rc = sqlite3_exec(db, sql , 0, this, &zErrMsg);
	if( rc != SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
}

void SqliteWorkHorse::initDatabase(){

	int rc = sqlite3_open_v2(this->plow->getDatabaseFile().c_str(), &db,  SQLITE_OPEN_FULLMUTEX | SQLITE_OPEN_READWRITE,0);
	if( rc ){
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		return;
	}
	sqlite3_enable_load_extension(db,1);
	execute_query("SELECT load_extension('mod_spatialite')");
	execute_query("SELECT load_extension('/Users/christianurich/Documents/Dynamind-Toolbox/build/release/output/Modules/libdm_sqlite_plugin')");
}

std::string SqliteWorkHorse::getMainTable()
{
	return this->main_table;
}

void SqliteWorkHorse::register_result(std::string r)
{
	result_vector.push_back(r);
}

void SqliteWorkHorse::run()
{

	initDatabase();

	std::stringstream sql_stream;

	DM::Logger(DM::Debug) << "running chunk " << id << "/" << this->plow->getTotalChunks();
	sql_stream.str("");
	sql_stream.clear();
	sql_stream << this->plow->GetWorkerQuery() << " limit " << start << "," << end;

	this->execute_query(sql_stream.str().c_str(), true);

	sqlite3_close_v2(db);
	this->plow->register_results(result_vector);
	DM::Logger(DM::Debug) << "done chunk " << id << "/" << this->plow->getTotalChunks();
}

SqliteWorkHorse::~SqliteWorkHorse()
{
}
