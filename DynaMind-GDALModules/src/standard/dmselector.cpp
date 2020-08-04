#include "dmselector.h"


#include "gdalspatiallinking.h"
#include <sstream>
#include <ogr_feature.h>
#include <dmsimulation.h>
#include "../../3rdparty/sqlite3/sqlite3.h"

DM_DECLARE_CUSTOM_NODE_NAME(DMSelector, Selector, Linking)

DMSelector::DMSelector()
{
	GDALModule = true;
	SQLExclusive = true;
	properInit = false;

	this->leadingViewName = "";
	this->addParameter("leadingViewName", DM::STRING, &leadingViewName);

	this->linkViewName = "";
	this->addParameter("linkViewName", DM::STRING, &linkViewName);

	this->attributeName = "";
	this->addParameter("attributeName", DM::STRING, &attributeName);

	this->withCentroid = true;
	this->addParameter("withCentroid", DM::BOOL, &withCentroid);

	//dummy to get the ports
	std::vector<DM::ViewContainer> data;
	data.push_back(  DM::ViewContainer ("dummy", DM::SUBSYSTEM, DM::MODIFY) );
	this->addGDALData("city", data);
}

void DMSelector::init()
{
	properInit = false;
	if (this->leadingViewName.empty())
		return;
	if (this->leadingViewName.empty())
		return;
	if (this->attributeName.empty())
		return;
	std::map<std::string, DM::View> inViews = getViewsInStream()["city"];
	if (inViews.find(leadingViewName) == inViews.end()) {
		DM::Logger(DM::Warning) << "view " << leadingViewName << " not found in in stream";
		return;
	}
	if (inViews.find(linkViewName) == inViews.end()) {
		DM::Logger(DM::Warning) << "view " << linkViewName << " not found in in stream";
		return;
	}
	this->leadingView = DM::ViewContainer(leadingViewName, inViews[leadingViewName].getType(), DM::READ);

	this->linkView = DM::ViewContainer(linkViewName, inViews[linkViewName].getType(), DM::READ);

	this->linkView.addAttribute(attributeName, DM::Attribute::INT, DM::WRITE);

	std::vector<DM::ViewContainer *> data_stream;
	data_stream.push_back(&leadingView);
	data_stream.push_back(&linkView);

	this->registerViewContainers(data_stream);

	properInit = true;
}



void DMSelector::execute_query1(sqlite3 *db, const char *sql ) {
	char *zErrMsg = nullptr;

	int rc;
		rc = sqlite3_exec(db, sql , nullptr, nullptr, &zErrMsg);
	if( rc != SQLITE_OK ){
		std::cout <<  "SQL error: " << zErrMsg << std::endl;
		sqlite3_free(zErrMsg);
	}
}

std::string DMSelector::get_filter(sqlite3 *db) {

	std::stringstream query;
	query << "SELECT filters from " << this->leadingViewName << " LIMIT 1";

	sqlite3_stmt *stmt;
	const char *sql = query.str().c_str(); //"SELECT ID, Name FROM User";
	int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
	if (rc != SQLITE_OK) {
		DM::Logger(DM::Error) << "GDALSpatialLinking init failed";
		this->setStatus(DM::MOD_CHECK_ERROR);
		DM::Logger(DM::Error) <<  "SQL error: " << sqlite3_errmsg(db);
		return "";
	}

	std::string filter;
	while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
		const unsigned char *name = sqlite3_column_text(stmt, 0);
		filter = std::string(reinterpret_cast<const char*>(name));
	}
	if (rc != SQLITE_DONE) {
		DM::Logger(DM::Error) <<  "SQL error: " << sqlite3_errmsg(db);
	}
	sqlite3_finalize(stmt);

	return filter;
}



void DMSelector::run()
{
	if (!properInit) {
		DM::Logger(DM::Error) << "GDALSpatialLinking init failed";
		this->setStatus(DM::MOD_CHECK_ERROR);
		return;
	}

	//Spatialte Backend
	std::stringstream query_spatail_index;
	query_spatail_index << "SELECT CreateSpatialIndex('" << this->leadingViewName << "','GEOMETRY')";


	std::stringstream query;

	std::string lead_filter = leadingView.get_attribute_filter_sql_string();

	query << "UPDATE " << this->linkViewName << " ";
	query << "SET " << attributeName << "=(SELECT P.ogc_fid FROM " << leadingViewName << " as P ";
	query << "WHERE ST_WITHIN( CENTROID(" << linkViewName <<".Geometry), P.Geometry) == 1 AND P.ROWID IN (SELECT ROWID FROM SpatialIndex WHERE f_table_name ='";
	query << leadingViewName << "' AND search_frame = " << linkViewName << ".Geometry ORDER BY ROWID)"  ;
	if (!lead_filter.empty())
		query << " AND  P." << lead_filter;
	query << ")";




	sqlite3 *db;
	int rc =  sqlite3_open(this->leadingView.getDBID().c_str(), &db);
	if( rc ){
		DM::Logger(DM::Error) <<  "Can't open database: " << sqlite3_errmsg(db);
		return;
	}
	sqlite3_enable_load_extension(db,1);

	sqlite3_enable_load_extension(db,1);
	#ifdef WIN32
		execute_query1(db,"SELECT load_extension('mod_spatialite')");
	#else
		execute_query1(db,"SELECT load_extension('/usr/local/lib/mod_spatialite')");
	#endif

	std::string filter = linkView.get_attribute_filter_sql_string();
	std::string geometry_filter = this->get_filter(db);

	if (!filter.empty() || !geometry_filter.empty())
		query << " WHERE ";
	if (!filter.empty())
		query << filter;
	if (!filter.empty() && !geometry_filter.empty())
		query << " AND ";
	if (!geometry_filter.empty())
		query << geometry_filter;
	DM::Logger(DM::Standard) << query.str();


	execute_query1(db,query_spatail_index.str().c_str());

	execute_query1(db,query.str().c_str());
	sqlite3_close(db);

	DM::Logger(DM::Standard) << query.str();
	return;
}




