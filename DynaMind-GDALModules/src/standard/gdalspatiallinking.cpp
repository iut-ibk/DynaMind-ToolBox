#include "gdalspatiallinking.h"
#include <sstream>
#include <ogr_feature.h>
#include <dmsimulation.h>
#include "../../3rdparty/sqlite3/sqlite3.h"

DM_DECLARE_CUSTOM_NODE_NAME(GDALSpatialLinking, Spatial Linking, Linking)

GDALSpatialLinking::GDALSpatialLinking()
{
	GDALModule = true;
	SQLExclusive = true;
	properInit = false;

	this->leadingViewName = "";
	this->addParameter("leadingViewName", DM::STRING, &leadingViewName);

	this->linkViewName = "";
	this->addParameter("linkViewName", DM::STRING, &linkViewName);

	this->withCentroid = true;
	this->addParameter("withCentroid", DM::BOOL, &withCentroid);

	//dummy to get the ports
	std::vector<DM::ViewContainer> data;
	data.push_back(  DM::ViewContainer ("dummy", DM::SUBSYSTEM, DM::MODIFY) );
	this->addGDALData("city", data);
}

void GDALSpatialLinking::init()
{
	properInit = false;
	if (this->leadingViewName.empty())
		return;
	if (this->leadingViewName.empty())
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
	std::stringstream l_name;
	l_name << leadingViewName << "_id";
	link_name = l_name.str();
	this->linkView.addAttribute(link_name, leadingViewName, DM::WRITE);

	std::vector<DM::ViewContainer *> data_stream;
	data_stream.push_back(&leadingView);
	data_stream.push_back(&linkView);

	this->registerViewContainers(data_stream);

	properInit = true;
}

bool GDALSpatialLinking::checkCentroid(OGRGeometry* geo, OGRGeometry * lead_geo)
{
	OGRPoint ct;
	if (int error = geo->Centroid(&ct) != OGRERR_NONE) {
		DM::Logger(DM::Warning) << "error calculationg ct " << error;
		return false;
	}
	return ct.Within(lead_geo);
}

bool GDALSpatialLinking::checkIntersection(OGRGeometry* geo, OGRGeometry * lead_geo)
{
	return (geo->Within(lead_geo) || geo->Intersects(lead_geo));
}

void GDALSpatialLinking::execute_query1(sqlite3 *db, const char *sql ) {
	char *zErrMsg = 0;

	int rc;
		rc = sqlite3_exec(db, sql , 0, 0, &zErrMsg);
	if( rc != SQLITE_OK ){
		std::cout <<  "SQL error: " << zErrMsg << std::endl;
		sqlite3_free(zErrMsg);
	}
}



void GDALSpatialLinking::run()
{
	if (!properInit) {
		DM::Logger(DM::Error) << "GDALSpatialLinking init failed";
		this->setStatus(DM::MOD_CHECK_ERROR);
		return;
	}

	//Spatialte Backend
	if (this->getSimulation()->getSimulationConfig().getCoorindateSystem() != 0) {
		DM::Logger(DM::Debug) << "Use Spatialite Backend";
		//// TODO: leadingView.createSpatialIndex();
		std::stringstream query;

		std::string lead_filter = leadingView.get_attribute_filter_sql_string();

		query << "UPDATE " << this->linkViewName << " ";
		query << "SET " << leadingViewName << "_id=(SELECT P.ogc_fid FROM " << leadingViewName << " as P ";
		query << "WHERE ST_WITHIN( CENTROID(" << linkViewName <<".Geometry), P.Geometry) == 1 AND P.ROWID IN (SELECT ROWID FROM SpatialIndex WHERE f_table_name ='";
		query << leadingViewName << "' AND search_frame = " << linkViewName << ".Geometry ORDER BY ROWID)"  ;
		if (!lead_filter.empty())
			query << " AND  P." << lead_filter;
		query << ")";

		std::string filter = linkView.get_attribute_filter_sql_string();

		if (!filter.empty())
			query << " WHERE " << filter;


		sqlite3 *db;
		int rc =  sqlite3_open(this->leadingView.getDBID().c_str(), &db);
		if( rc ){
			DM::Logger(DM::Error) <<  "Can't open database: " << sqlite3_errmsg(db);
			return;
		}
		sqlite3_enable_load_extension(db,1);

		execute_query1(db,"SELECT load_extension('mod_spatialite')");

		DM::Logger(DM::Standard) << query.str();

		execute_query1(db,query.str().c_str());
		sqlite3_close(db);



		DM::Logger(DM::Standard) << query.str();
		return;
	}
	DM::Logger(DM::Standard) << "Use GDAL backend as fallback EPSG CODE to improve performace";
	//GDAL Backend
	leadingView.resetReading();

	OGRFeature * lead_feat = 0;
	while (lead_feat = leadingView.getNextFeature()) {

		OGRFeature * link_feature = 0;
		OGRGeometry * lead_geo = lead_feat->GetGeometryRef();
		int id = lead_feat->GetFID();

		if (!lead_geo) {
			DM::Logger(DM::Warning) << "Lead feature geometry is null";
			continue;
		}

		linkView.resetReading();
		linkView.setSpatialFilter(lead_geo);
		long counter = 0;
		while (link_feature = linkView.getNextFeature()) {

			OGRGeometry * geo = link_feature->GetGeometryRef();
			if (!geo){
				DM::Logger(DM::Warning) << "feature link feature geometry is null";
				continue;
			}
			bool isLinked = false;
			if (withCentroid)
				isLinked = checkCentroid(geo, lead_geo);
			else
				isLinked = checkIntersection(geo, lead_geo);
			if (!isLinked)
				continue;
			link_feature->SetField(link_name.c_str(), id);
			counter++;
		}
		if (counter % 100000){
			linkView.syncAlteredFeatures();
			leadingView.syncReadFeatures();
		}
	}
}

std::string GDALSpatialLinking::getHelpUrl()
{
	return "/DynaMind-GDALModules/gdalspatiallinking.html";
}
