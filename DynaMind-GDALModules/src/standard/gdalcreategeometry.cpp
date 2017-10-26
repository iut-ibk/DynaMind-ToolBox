#include "gdalcreategeometry.h"

#include <sstream>


DM_DECLARE_CUSTOM_NODE_NAME(GDALCreateGeometry, Create Centroids, Geometry Processing)


void GDALCreateGeometry::execute_query(const char *sql, bool cb ) {
	char *zErrMsg = 0;

	int rc;
	if (cb) {
		rc = sqlite3_exec(db, sql , callback, this, &zErrMsg);
	} else {
		rc = sqlite3_exec(db, sql , 0, this, &zErrMsg);
	}
	if( rc != SQLITE_OK ){
		DM::Logger(DM::Error) <<  "SQL error: " << zErrMsg;
		sqlite3_free(zErrMsg);
	}
}


void GDALCreateGeometry::initDatabase(){

	int rc = sqlite3_open(this->resultView.getDBID().c_str(), &db);
	if( rc ){
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		return;
	}
	sqlite3_enable_load_extension(db,1);
	execute_query("SELECT load_extension('mod_spatialite')", false);

	#ifdef WIN32
		execute_query("SELECT load_extension('mod_spatialite')", false);
	#else
		execute_query("SELECT load_extension('/usr/local/lib/mod_spatialite')", false);
	#endif

}

 bool GDALCreateGeometry::checkIsFlat(int ogrType)
{
	bool isFlat;
	switch(wkbFlatten(ogrType))
	{
	case wkbPoint:
		isFlat = true;
		break;
	case wkbPolygon:
		isFlat = true;
		break;
	case wkbMultiPolygon:
		isFlat=false;
		break;
	case wkbLineString:
		isFlat = true;
		break;
	case wkbMultiLineString:
		isFlat = false;
		break;
	case wkbNone:
		isFlat = true;
		break;
	default:
		isFlat = false;
	}
	return isFlat;
}

int GDALCreateGeometry::callback(void *db_w, int argc, char **argv, char **azColName){
	if (argc == 0  )
		return 0;

	GDALCreateGeometry* db_worker = reinterpret_cast<GDALCreateGeometry*>(db_w);


	OGRGeometry * geo_ref = OGRGeometryFactory::createGeometry(db_worker->getType());
	OGRGeometryFactory::createFromWkt(argv,db_worker->getView()->getSpatialReference(), &geo_ref);

	//OGRMultiPolygon * geo = (OGRMultiPolygon*) geo_ref;
	if (!geo_ref)
		return 0;

	OGRGeometry * geo_single = 0;
	std::vector<OGRGeometry*> geo_collection;

	if (!GDALCreateGeometry::checkIsFlat(geo_ref->getGeometryType())) {
		if (db_worker->getType() == DM::FACE) {
			OGRMultiPolygon * geo = (OGRMultiPolygon*) geo_ref;
			if (!geo)
				return 0;
			if (geo->getNumGeometries() == 0)
				return 0;
			geo_single = geo->getGeometryRef(0);//OGRGeometryFactory::forceToPolygon(geo);
			for (int i = 0; i < geo->getNumGeometries(); i++) {
				geo_collection.push_back(geo->getGeometryRef(i));
			}

		}
		if (db_worker->getType() == DM::EDGE) {
			OGRMultiLineString * geo = (OGRMultiLineString*) geo_ref;
			if (!geo)
				return 0;
			if (geo->getNumGeometries() == 0)
				return 0;
			geo_single = geo->getGeometryRef(0);
			for (int i = 0; i < geo->getNumGeometries(); i++) {
				geo_collection.push_back(geo->getGeometryRef(i));
			}
		}
		if (db_worker->getType() == DM::NODE) {
			OGRMultiPoint * geo = (OGRMultiPoint*) geo_ref;
			if (!geo)
				return 0;
			if (geo->getNumGeometries() == 0)
				return 0;
			geo_single = geo->getGeometryRef(0);
			for (int i = 0; i < geo->getNumGeometries(); i++) {
				geo_collection.push_back(geo->getGeometryRef(i));
			}
		}
	} else {
		geo_single = geo_ref;
		geo_collection.push_back(geo_single);
	}

	foreach(OGRGeometry * g, geo_collection){
		OGRFeature * f_new =  db_worker->getView()->createFeature();
				f_new->SetGeometry(g);
			}
	return 0;
}



GDALCreateGeometry::GDALCreateGeometry()
{
	GDALModule = true;

	resultViewName = "";
	this->addParameter("result_view", DM::STRING, &this->resultViewName);

	sqlQuery = "";
	this->addParameter("sqlQuery", DM::STRING, &this->sqlQuery);

	append = false;
	this->addParameter("append", DM::BOOL, &append);

	this->geometry_type = 0;
	this->addParameter("geometry_type", DM::INT, &geometry_type);



}
void GDALCreateGeometry::init()
{
	if(resultViewName.empty())
		return;

	resultView = DM::ViewContainer(this->resultViewName, this->geometry_type, DM::WRITE);

	std::vector<DM::ViewContainer*> views;

	if (append) {
		dummy_view = DM::ViewContainer("dummy", DM::SUBSYSTEM, DM::MODIFY);
		views.push_back(&dummy_view);
	}
	views.push_back(&resultView);

	this->registerViewContainers(views);

	switch ( this->geometry_type ) {
	case DM::COMPONENT:
		wkbType = wkbNone;
		break;
	case DM::NODE:
		wkbType = wkbPoint;
		break;
	case DM::EDGE:
		wkbType = wkbLineString;
		break;
	case DM::FACE:
		wkbType = wkbPolygon;
		break;
	}


}

string GDALCreateGeometry::getHelpUrl()
{
	return "/DynaMind-GDALModules/gdalcalculatecentroids.html";
}

DM::ViewContainer *GDALCreateGeometry::getView()
{
	return &this->resultView;
}

OGRwkbGeometryType GDALCreateGeometry::getType()
{
	return this->wkbType;
}


void GDALCreateGeometry::run()
{

	initDatabase();

	std::stringstream sql_stream;

	sql_stream << this->sqlQuery;

	DM::Logger(DM::Standard) << sql_stream.str().c_str();
	this->execute_query(sql_stream.str().c_str(), true);

	sqlite3_close(db);
}



