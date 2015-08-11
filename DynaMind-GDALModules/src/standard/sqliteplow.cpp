#include "sqliteworkhorse.h"

#include "sqliteplow.h"

#include <iostream>
#include <sstream>

#include <QMutexLocker>
#include <QThreadPool>

#include <dmlogger.h>


void SqlitePlow::execute_query(sqlite3 *db, const char *sql, bool cb ) {
	char *zErrMsg = 0;

	int rc;
	if (cb) {
		rc = sqlite3_exec(db, sql , callback, this, &zErrMsg);
	} else {
		rc = sqlite3_exec(db, sql , 0, this, &zErrMsg);
	}
	if( rc != SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
}

int SqlitePlow::callback(void *plow, int argc, char **argv, char **azColName){


	SqlitePlow * p = reinterpret_cast<SqlitePlow*>(plow);
	p->setStillMore(true);

	return 0;
}


SqlitePlow::SqlitePlow(QObject *parent): junk_size(5000), QObject(parent)
{

}

void SqlitePlow::commit_result(std::string uq)
{
	QMutexLocker ml(&mMutex);
	this->results_vector.push_back(uq);
}

void SqlitePlow::register_results(std::vector<std::string> vr)
{
	QMutexLocker ml(&mMutex);
	this->chunk_results.push_back(vr);
	done_counter++;
	DM::Logger(DM::Standard) << "done " << done_counter << "/" << chunk_counter;
}

void SqlitePlow::setStillMore(bool v)
{
	stillMore = v;
}

void SqlitePlow::setWorkerQuery(std::string query)
{
	this->workerQuery = query;
}

std::string SqlitePlow::GetWorkerQuery()
{
	return this->workerQuery;
}

void SqlitePlow::setMainTable(std::string main_table)
{
	this->main_table = main_table;
}

std::string SqlitePlow::GetMainTable()
{
	return this->main_table;
}

int SqlitePlow::getTotalChunks()
{
	return this->chunk_counter;
}

void SqlitePlow::setJunkSize(int size)
{
	this->junk_size = size;
}

void SqlitePlow::setDatabaseFile(std::string database_file)
{
	this->database_file = database_file;
}

std::string SqlitePlow::getDatabaseFile()
{
	return this->database_file;
}

void SqlitePlow::plow() {

	sqlite3 *db;
	int rc = sqlite3_open_v2(this->database_file.c_str(), &db,  SQLITE_OPEN_FULLMUTEX | SQLITE_OPEN_READWRITE,0);
	if( rc ){
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		return;
	}
	sqlite3_enable_load_extension(db,1);
	this->execute_query(db,"SELECT load_extension('mod_spatialite')");
	this->execute_query(db,"SELECT load_extension('/Users/christianurich/Documents/Dynamind-Toolbox/build/release/output/Modules/libdm_sqlite_plugin')");



	QThreadPool qpool;
	chunk_counter = 0;
	done_counter= 0;
	do {
		//std::cout << chunck_size*(i-1) << std::endl;
		this->stillMore = false;
		// Check if chunck exists;
		std::stringstream sql_stream;
		sql_stream << "SELECT ogc_fid from parcel limit " << junk_size*(chunk_counter+1) << "," <<  1;

		//std::cout << "Start Chunk" << sql_stream.str() << std::endl;
		SqliteWorkHorse * worker = new SqliteWorkHorse(this, chunk_counter,(junk_size*(chunk_counter)), junk_size);
		qpool.start(worker);

		this->execute_query(db, sql_stream.str().c_str(), true);
		chunk_counter++;
	} while (stillMore);
	DM::Logger(DM::Standard) << "Started " << chunk_counter << " chunks";
	qpool.waitForDone();



	std::stringstream query;

	DM::Logger(DM::Standard) << "Syncronise DB";

	for(int i = 0; i < this->chunk_results.size(); i++) {
		query << "BEGIN TRANSACTION;\n";
		foreach(std::string r, this->chunk_results[i]){
			query << r << ";\n";
		}
		query << "COMMIT;\n";
	}

	//std::cout << query.str() << std::endl;
	execute_query(db,query.str().c_str());
	DM::Logger(DM::Standard) << "End Syncronise DB";
	sqlite3_close_v2(db);

	emit finished();
}
