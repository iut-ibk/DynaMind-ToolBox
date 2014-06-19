#include "dmgdalsystem.h"

#include <dmlogger.h>
#include <dmattribute.h>
#include <dmsystem.h>

#include <sstream>

#include <QUuid>

namespace DM {

GDALSystem::GDALSystem()
{
	//Init SpatialiteServer
	OGRRegisterAll();

	poDrive = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName( "SQLite" );
	char ** options = NULL;
	options = CSLSetNameValue( options, "OGR_SQLITE_CACHE", "1024" );

	DBID = QUuid::createUuid().toString();

	QString dbname =  "/tmp/" + DBID + ".db";

	poDS = poDrive->CreateDataSource(dbname.toStdString().c_str() , options );

	if( poDS == NULL ) {
		DM::Logger(DM::Error) << "couldn't create source";
	}

	//Create State ID
	this->state_ids.push_back(QUuid::createUuid().toString().toStdString());

	//Init uuid
	latestUniqueId = 0;
	predecessor = NULL;
}

GDALSystem::GDALSystem(const GDALSystem &s)
{
	//Copy all that is needed
	poDrive = s.poDrive;
	viewLayer = s.viewLayer;
	state_ids = s.state_ids;
	state_ids = s.state_ids;
	uniqueIdsTonfid = s.uniqueIdsTonfid;
	latestUniqueId = s.latestUniqueId;
	sucessors = std::vector<DM::GDALSystem*>();
	DBID = QUuid::createUuid().toString();

	//Copy DB
	QString origin =  "/tmp/" + s.DBID + ".db";
	QString dest = "/tmp/" + DBID + ".db";
	QFile::copy(origin, dest);

	//Connect to DB
	char ** options = NULL;
	options = CSLSetNameValue( options, "OGR_SQLITE_CACHE", "1024" );

	poDS = poDrive->Open(dest.toStdString().c_str(), true);

	//Create new state
	state_ids.push_back(QUuid::createUuid().toString().toStdString());

	//RebuildViewLayer
	for (std::map<std::string, OGRLayer *>::const_iterator it = viewLayer.begin();
		 it != viewLayer.end(); ++it) {
		std::string viewname = it->first;
		viewLayer[it->first] = poDS->GetLayerByName(viewname.c_str());
	}
}


void GDALSystem::updateView(const View &v)
{
	//if view is not in map create a new ogr layer
	if (viewLayer.find(v.getName()) == viewLayer.end()) {
		OGRLayer * lyr = this->createLayer(v);

		if (lyr == NULL) {
			DM::Logger(DM::Error) << "couldn't create layer";
		}

		OGRFieldDefn oField_id( "dynamind_id", OFTInteger );
		lyr->CreateField(&oField_id);

		OGRFieldDefn oField_state_id( "dynamind_state_id", OFTString );
		lyr->CreateField(&oField_state_id);
		viewLayer[v.getName()] = lyr;
	}

	OGRLayer * lyr = viewLayer[v.getName()];
	//Update Features
	foreach(std::string attribute_name, v.getAllAttributes()) {
		//Feature alredy in layer
		if (lyr->FindFieldIndex(attribute_name.c_str(), true) != -1)
			continue;

		if (v.getAttributeType(attribute_name) == DM::Attribute::STRING){
			OGRFieldDefn oField ( attribute_name.c_str(), OFTString );
			lyr->CreateField(&oField);
			continue;
		}
		if (v.getAttributeType(attribute_name) == DM::Attribute::DOUBLE){
			OGRFieldDefn oField ( attribute_name.c_str(), OFTReal );
			lyr->CreateField(&oField);
			continue;
		}
	}
}

OGRFeature *GDALSystem::createFeature(const View &v)
{
	OGRLayer * lyr = viewLayer[v.getName()];
	OGRFeature * f = OGRFeature::CreateFeature(lyr->GetLayerDefn());
	f->SetField("dynamind_id", (int) latestUniqueId++);
	f->SetField("dynamind_state_id", this->state_ids[state_ids.size()-1].c_str());
	return f;
}

OGRLayer *GDALSystem::getOGRLayer(const View &v)
{
	if (viewLayer.find(v.getName()) == viewLayer.end()) {
		Logger(Error) << "Layer not found";
		return 0;
	}
	return viewLayer[v.getName()];
}

OGRDataSource *GDALSystem::getDataSource()
{
	return this->poDS;
}

void GDALSystem::resetReading(const View &v)
{
	if (viewLayer.find(v.getName()) == viewLayer.end()) {
		Logger(Error) << "Layer not found";
		return;
	}
	OGRLayer * lyr = viewLayer[v.getName()];
	lyr->ResetReading();

//	std::stringstream state_filter;
//	for (int i = 0; i < this->state_ids.size(); i++) {
//		if (i != 0) {
//			state_filter << " or ";
//		}
//		state_filter << "dynamind_state_id = '" << this->state_ids[i] << "'";
//	}
//	Logger(Debug) << state_filter.str();

//	lyr->SetAttributeFilter(state_filter.str().c_str());




}

GDALSystem *GDALSystem::createSuccessor()
{
	Logger(Debug) << "Create Sucessor ";
	GDALSystem* result = new GDALSystem(*this);
	this->sucessors.push_back(result);
	result->setPredecessor(this);

	return result;
}

OGRFeature *GDALSystem::getFeature(const DM::View & v, long dynamind_id)
{
	if (viewLayer.find(v.getName()) == viewLayer.end()) {
		Logger(Error) << "Layer not found";
		return NULL;
	}
	OGRLayer * lyr = viewLayer[v.getName()];

	return lyr->GetFeature(this->uniqueIdsTonfid[dynamind_id]);
}

void GDALSystem::updateViews(const std::vector<View> &views)
{
	foreach (DM::View v, views) {
		this->updateView(v);
	}
}

GDALSystem *GDALSystem::getPredecessor() const
{
	return this->predecessor;
}

void GDALSystem::setPredecessor(GDALSystem * sys)
{
	this->predecessor = sys;
}

OGRFeature *GDALSystem::getNextFeature(const View &v)
{
	OGRLayer * lyr = viewLayer[v.getName()];
	return lyr->GetNextFeature();
}

string GDALSystem::getCurrentStateID()
{
	return this->state_ids[state_ids.size()-1];
}

OGRLayer *GDALSystem::createLayer(const View &v)
{
	switch ( v.getType() ) {
	case DM::COMPONENT:
		return poDS->CreateLayer(v.getName().c_str(), NULL, wkbNone, NULL );
		break;
	case DM::NODE:
		return poDS->CreateLayer(v.getName().c_str(), NULL, wkbPoint, NULL );
		break;
	case DM::EDGE:
		return poDS->CreateLayer(v.getName().c_str(), NULL, wkbLineString, NULL );
		break;
	case DM::FACE:
		return poDS->CreateLayer(v.getName().c_str(), NULL, wkbPolygon, NULL );
		break;
	}

	return NULL;
}

void GDALSystem::syncNewFeatures(const DM::View & v, std::vector<OGRFeature *> & df)
{
	OGRLayer * lyr = viewLayer[v.getName()];
	//Sync all features
	int counter = 0;
	int global_counter = 0;
	lyr->StartTransaction();
	foreach (OGRFeature * f, df) {
		counter++;
		global_counter++;
		if (counter == 10000) {
			lyr->CommitTransaction();
			lyr->StartTransaction();
			counter=0;
		}
		if (!f)
			continue;
		lyr->CreateFeature(f);
		uniqueIdsTonfid.push_back(f->GetFID());
		OGRFeature::DestroyFeature(f);
	}
	lyr->CommitTransaction();
	df.clear();
}

void GDALSystem::syncAlteredFeatures(const DM::View & v, std::vector<OGRFeature *> & df)
{
	OGRLayer * lyr = viewLayer[v.getName()];
	//Sync all features
	int counter = 0;
	int global_counter = 0;
	lyr->StartTransaction();
	foreach (OGRFeature * f, df) {
		counter++;
		global_counter++;
		if (counter == 100000) {
			lyr->CommitTransaction();
			lyr->StartTransaction();
			counter=0;
		}
		if (!f)
			continue;
		lyr->SetFeature(f);
		OGRFeature::DestroyFeature(f);
	}
	lyr->CommitTransaction();
	df.clear();
}

}
