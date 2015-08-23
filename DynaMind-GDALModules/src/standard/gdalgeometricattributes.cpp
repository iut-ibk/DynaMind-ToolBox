#include "gdalgeometricattributes.h"
#include "geometricattributeworker.h"
#include "dmsimulation.h"
#include "sqliteplow.h"

#include <ogr_api.h>
#include <sqlite3.h>

#include <sstream>

int execute_sql_statement(sqlite3 *db, const char *sql){
	/* Execute SQL statement */
	const char* data = "Callback function called";
	char *zErrMsg = 0;

	int rc = sqlite3_exec(db, sql, 0, (void*)data, &zErrMsg);
	if( rc != SQLITE_OK ){
		DM::Logger(DM::Error) <<" SQL statment: " << QString::fromLatin1(sql).toStdString();
		DM::Logger(DM::Error) <<" SQL error: " << QString::fromLatin1(zErrMsg).toStdString();
	   sqlite3_free(zErrMsg);
	}

	return rc;
}

DM_DECLARE_CUSTOM_NODE_NAME(GDALGeometricAttributes, Calculate Feature Area, Data Handling)

GDALGeometricAttributes::GDALGeometricAttributes()
{
	GDALModule = true;

	this->leadingViewName = "";
	this->addParameter("leading_view", DM::STRING, &this->leadingViewName);

	this->isCalculateArea = true;
	this->addParameter("calculate_area", DM::BOOL, &this->isCalculateArea);

	this->isAspectRationBB = false;
	this->addParameter("aspect_ratio_BB", DM::BOOL, &this->isAspectRationBB);

	this->isPercentageFilled = false;
	this->addParameter("percentage_filled", DM::BOOL, &this->isPercentageFilled);

	this->useSQL = false;
	this->addParameter("useSQL", DM::BOOL, &this->useSQL);

}

void GDALGeometricAttributes::init()
{
	if (leadingViewName.empty())
		return;
	vc = DM::ViewContainer(this->leadingViewName, DM::FACE, DM::READ);
	if (isCalculateArea)
		vc.addAttribute("area", DM::Attribute::DOUBLE, DM::WRITE);
	if (isAspectRationBB)
		vc.addAttribute("aspect_ratio_BB", DM::Attribute::DOUBLE, DM::WRITE);
	if (isPercentageFilled)
		vc.addAttribute("percentage_filled", DM::Attribute::DOUBLE, DM::WRITE);

	std::vector<DM::ViewContainer*> data_stream;
	data_stream.push_back(&vc);

	this->registerViewContainers(data_stream);
}

void GDALGeometricAttributes::run_sql()
{
	sqlite3 *db;
	int rc = sqlite3_open(vc.getDBID().c_str(), &db);
	sqlite3_enable_load_extension(db,1);

	if( rc ){
		DM::Logger(DM::Error) << "Failed to open database";
		return;
	}

	//Load spatialite
	execute_sql_statement(db, "SELECT load_extension('mod_spatialite')");

	std::stringstream query_stream;
	query_stream << "SELECT load_extension('" << this->getSimulation()->getSimulationConfig().getDefaultModulePath() << "/SqliteExtension/libdm_sqlite_plugin" << "')";

	execute_sql_statement(db, query_stream.str().c_str());

	query_stream.str("");
	query_stream.clear();
	query_stream << "UPDATE " << this->leadingViewName <<  " set ";

	if (isCalculateArea)
		query_stream << "area = area(geometry), ";
	if (isAspectRationBB)
		query_stream << "aspect_ratio_BB = dm_poly_aspect_ratio(ASWKT(geometry)), ";
	if (isPercentageFilled)
		query_stream << "percentage_filled = dm_poly_percentage_filled(ASWKT(geometry)) ";
	DM::Logger(DM::Standard) << query_stream.str();
	execute_sql_statement(db, query_stream.str().c_str());

	sqlite3_close(db);
}

void GDALGeometricAttributes::run_sql_threaded()
{


	std::stringstream query_stream;
	query_stream << "SELECT ogc_fid ";
	if (isCalculateArea)
		query_stream << ", area(geometry)  as area ";
	if (isAspectRationBB)
		query_stream << ", dm_poly_aspect_ratio(ASWKT(geometry)) as aspect_ratio_bb   ";
	if (isPercentageFilled)
		query_stream << ", dm_poly_percentage_filled(ASWKT(geometry)) as percentage_filled ";

	query_stream << " from " << this->leadingViewName;
	// "SELECT ogc_fid, area(geometry)  as area, dm_poly_percentage_filled(ASWKT(geometry)) as percentage_filled , dm_poly_aspect_ratio(ASWKT(geometry)) as aspect_ratio_bb from parcel "

	//QString location_plugins = QString::fromStdString(this->getSimulation()->getSimulationConfig().getDefaultModulePath())+ QString::fromStdString("/SqliteExtension/libdm_sqlite_plugin");
#ifdef THREADING
	SqlitePlow plower(this->getSimulation()->getSimulationConfig().getDefaultModulePath() + "/SqliteExtension/libdm_sqlite_plugin", this->vc.getDBID(),query_stream.str(),this->leadingViewName);

	plower.plow();
#endif

}

string GDALGeometricAttributes::getHelpUrl()
{
	return "/DynaMind-GDALModules/gdalgeometricattributes.html";
}

void GDALGeometricAttributes::run()
{

	//return;
	if (useSQL) {
		//run_sql();
		run_sql_threaded();
		return;
	}

	OGRFeature * f;

	vc.resetReading();
	QThreadPool pool;
	std::vector<OGRFeature*> container;
	int counter = 0;
	while( f = vc.getNextFeature() ) {
		container.push_back(f);
		if (counter%10000 == 0){
			GeometricAttributeWorker * gw = new GeometricAttributeWorker(this, container, isCalculateArea, isAspectRationBB , isPercentageFilled);
			pool.start(gw);
			container = std::vector<OGRFeature*>();
		}
	}
	pool.waitForDone();
}

