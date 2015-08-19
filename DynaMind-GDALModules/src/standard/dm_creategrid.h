#ifndef DM_CREATEGRID_H
#define DM_CREATEGRID_H


#include <dmmodule.h>
#include <dm.h>

#include <sqlite3.h>
class DM_HELPER_DLL_EXPORT DM_CreateGrid : public DM::Module
{
DM_DECLARE_NODE(DM_CreateGrid)
public:
	DM_CreateGrid();
	void run();
	void init();

	DM::ViewContainer *getGridView();
private:
	sqlite3 *db;
	std::string lead_view_name;
	std::string grid_view_name;

	DM::ViewContainer lead_view;
	DM::ViewContainer grid_view;



	void execute_query(const char *sql, bool cb );
	static int callback(void *db_worker, int argc, char **argv, char **azColName);

	void initDatabase();
};

#endif // DM_CREATEGRID_H
