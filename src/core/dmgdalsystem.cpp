#include "dmgdalsystem.h"

#include <dmlogger.h>
#include <dmattribute.h>

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
	QString dbname = "/tmp/" + QUuid::createUuid().toString() + ".db";
	poDS = poDrive->CreateDataSource(dbname.toStdString().c_str() , options );

	if( poDS == NULL ) {
		DM::Logger(DM::Error) << "couldn't create source";
	}

	//Create State ID
	this->state_ids.push_back(QUuid::createUuid().toString().toStdString());
}

GDALSystem::GDALSystem(const GDALSystem &s)
{
	//Copy all that is needed
	poDS = s.poDS;
	poDrive = s.poDrive;
	viewLayer = s.viewLayer;
	state_ids = s.state_ids;
	dirtyFeatures = s.dirtyFeatures;
	state_ids = s.state_ids;

	//Create new state
	state_ids.push_back(QUuid::createUuid().toString().toStdString());
}

void GDALSystem::updateSystemView(const View &v)
{
	//if view is not in map create a new ogr layer
	if (viewLayer.find(v.getName()) == viewLayer.end()) {
		Logger(Debug) << "Create Layer";
		OGRLayer * lyr = poDS->CreateLayer("components", NULL, wkbNone, NULL );

		if (lyr == NULL) {
			DM::Logger(DM::Error) << "couldn't create layer";
		}
		OGRFieldDefn oField_id( "dynamind_id", OFTString );
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
	}
}

OGRFeature *GDALSystem::createFeature(const View &v)
{
	OGRLayer * lyr = viewLayer[v.getName()];
	OGRFeature * f = OGRFeature::CreateFeature(lyr->GetLayerDefn());
	f->SetField("dynamind_id", QUuid::createUuid().toString().toStdString().c_str());
	f->SetField("dynamind_state_id", this->state_ids[state_ids.size()-1].c_str());

	dirtyFeatures.push_back(f);
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
	}
	OGRLayer * lyr = viewLayer[v.getName()];
	lyr->ResetReading();

	std::stringstream state_filter;
	for (int i = 0; i < this->state_ids.size(); i++) {
		if (i != 0)
			state_filter << " or ";
		state_filter << "dynamind_state_id = '" << this->state_ids[i] << "'";
	}
	Logger(Debug) << state_filter.str();

	lyr->SetAttributeFilter(state_filter.str().c_str());
}

void GDALSystem::syncFeatures(const DM::View & v)
{
	OGRLayer * lyr = viewLayer[v.getName()];
	//Sync all features
	int counter = 0;
	int global_counter = 0;
	lyr->StartTransaction();
	foreach (OGRFeature * f, dirtyFeatures) {
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
		OGRFeature::DestroyFeature(f);
	}
	lyr->CommitTransaction();

	dirtyFeatures.clear();
}

}
