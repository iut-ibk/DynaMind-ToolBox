#ifndef GDALCREATEGEOMETRY_H
#define GDALCREATEGEOMETRY_H
#include <dmmodule.h>
#include <dm.h>
#include <ogrsf_frmts.h>
#include "../../3rdparty/sqlite3/sqlite3.h"


class DM_HELPER_DLL_EXPORT GDALCreateGeometry : public DM::Module
{
	DM_DECLARE_NODE(GDALCreateGeometry)
public:
	GDALCreateGeometry();

	void run();
	void init();
	std::string getHelpUrl();

private:

	DM::ViewContainer *getView();
	OGRwkbGeometryType getType();

	sqlite3 *db;

	bool append;

	std::string resultViewName;
	std::string sqlQuery;
	int geometry_type;
	OGRwkbGeometryType wkbType;

	DM::ViewContainer resultView;
	DM::ViewContainer dummy_view;

	void execute_query(const char *sql, bool cb);
	void initDatabase();
	static int callback(void *db_w, int argc, char **argv, char **azColName);
	static bool checkIsFlat(int ogrType);
};



#endif // GDALCREATEGEOMETRY_H
