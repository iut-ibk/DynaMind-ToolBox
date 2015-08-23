#include "dm_creategrid.h"
#include <dmsimulation.h>
#include <ogrsf_frmts.h>

#include <sstream>

DM_DECLARE_CUSTOM_NODE_NAME(DM_CreateGrid, Create Grid, Data Handling)

int DM_CreateGrid::callback(void *db_w, int argc, char **argv, char **azColName){
	if (argc == 0)
		return 0;

	DM_CreateGrid* db_worker = reinterpret_cast<DM_CreateGrid*>(db_w);


	std::vector<std::string> values(argv , argv + argc);
	std::vector<std::string> names(azColName, azColName + argc);

	OGRGeometry * geo_ref = OGRGeometryFactory::createGeometry(wkbMultiPolygon);
	OGRGeometryFactory::createFromWkt(argv,db_worker->getGridView()->getSpatialReference(), &geo_ref);

	OGRMultiPolygon * geo = (OGRMultiPolygon*) geo_ref;
	if (!geo)
		return 0;
	if (geo->getNumGeometries() == 0)
		return 0;
	//geo_single = geo->getGeometryRef(0);//OGRGeometryFactory::forceToPolygon(geo);
	int polygons = geo->getNumGeometries();
	for (int i = 0; i < geo->getNumGeometries(); i++) {
		//geo_collection.push_back(geo->getGeometryRef(i));
		OGRFeature * f = db_worker->getGridView()->createFeature();
		f->SetGeometry(geo->getGeometryRef(i));
	}
	DM::Logger(DM::Standard) << polygons;

	return 0;
}


DM_CreateGrid::DM_CreateGrid()
{
	this->GDALModule = true;

	this->addParameter("lead_view_name", DM::STRING, &this->lead_view_name);
	this->lead_view_name = "boundary";

	this->addParameter("grid_view_name", DM::STRING, &this->grid_view_name);
	this->grid_view_name = "grid";

	this->addParameter("grid_size", DM::DOUBLE, &this->grid_size);
	this->grid_size = 100;




	//dummy to get the ports
	std::vector<DM::ViewContainer> data;
	data.push_back(  DM::ViewContainer ("dummy", DM::SUBSYSTEM, DM::MODIFY) );
	this->addGDALData("city", data);
}

void DM_CreateGrid::run()
{
	initDatabase();

	std::stringstream sql_stream;

	sql_stream << "SELECT ASWKT(ST_SquareGrid(geometry, " << grid_size << ")) FROM " << this->lead_view_name;

	std::cout << sql_stream.str().c_str() << std::endl;
	this->execute_query(sql_stream.str().c_str(), true);



	sqlite3_close_v2(db);
}

void DM_CreateGrid::execute_query(const char *sql, bool cb ) {
	char *zErrMsg = 0;

	int rc;
	if (cb) {
		rc = sqlite3_exec(db, sql , callback, this, &zErrMsg);
	} else {
		rc = sqlite3_exec(db, sql , 0, this, &zErrMsg);
	}
	if( rc != SQLITE_OK ){
		std::cout <<  "SQL error: " << zErrMsg << std::endl;
		sqlite3_free(zErrMsg);
	}
}

void DM_CreateGrid::initDatabase(){

	int rc = sqlite3_open_v2(this->lead_view.getDBID().c_str(), &db,  SQLITE_OPEN_FULLMUTEX | SQLITE_OPEN_READONLY,0);
	if( rc ){
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		return;
	}
	sqlite3_enable_load_extension(db,1);
	execute_query("SELECT load_extension('mod_spatialite')", false);
	/*std::stringstream query_stream;
	query_stream << "SELECT load_extension('" << this->getSimulation()->getSimulationConfig().getDefaultModulePath() << '/SqliteExtension/libdm_sqlite_plugin' << "')";

	execute_query(query_stream.str().c_str(), false);*/
}

void DM_CreateGrid::init()
{
	std::vector<DM::ViewContainer*> data;


	if (!this->lead_view_name.empty())	{
		lead_view = DM::ViewContainer(this->lead_view_name, DM::FACE, DM::READ);
		data.push_back(&lead_view);

	}

	if (!this->grid_view_name.empty())	{
		grid_view = DM::ViewContainer(this->grid_view_name, DM::FACE, DM::WRITE);
		data.push_back(&grid_view);
	}

	this->registerViewContainers(data);
}

DM::ViewContainer *DM_CreateGrid::getGridView()
{
	return &grid_view;
}
