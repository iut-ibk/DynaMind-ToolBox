#include "gdalhotstarter.h"

#include <ogrsf_frmts.h>
#include "gdalutilities.h"
#include <dmgdalsystem.h>


DM_DECLARE_NODE_NAME(GDALHotStarter,Data Import and Export)

DM::ViewContainer *GDALHotStarter::viewContainerFactory(OGRLayer *lyr)
{
	OGRFeatureDefn * def = lyr->GetLayerDefn();
	int dm_geometry = DM::GDALUtilities::OGRtoDMGeometry(def);
	std::map<std::string, std::string> translator;
	DM::ViewContainer * view = new DM::ViewContainer(lyr->GetName(), dm_geometry, DM::WRITE);
	for (int i = 0; i < def->GetFieldCount(); i++){
		OGRFieldDefn * fdef = def->GetFieldDefn(i);
		//DM Datatype
		DM::Attribute::AttributeType type = DM::GDALUtilities::OGRToDMAttribute(fdef);
		if (type == DM::Attribute::NOTYPE)
			continue;
		DM::Logger(DM::Debug) << "Load attribute" << fdef->GetNameRef();

		std::string attribute_name = fdef->GetNameRef();
		std::transform(attribute_name.begin(), attribute_name.end(), attribute_name.begin(), ::tolower);

		translator[attribute_name] = fdef->GetNameRef();

		view->addAttribute(attribute_name.c_str(), type, DM::WRITE);
	}

	return view;
}

GDALHotStarter::GDALHotStarter()
{
	GDALModule = true;
	OGRRegisterAll();

	this->hotStartDatabase = "";
	this->addParameter("hot_start_database", DM::FILENAME, &hotStartDatabase);
}

void GDALHotStarter::init()
{

	OGRDataSource *poDS = OGRSFDriverRegistrar::Open(this->hotStartDatabase.c_str());
	if (!poDS) {
		DM::Logger(DM::Warning) << "Error loading " << this->hotStartDatabase;
		return;
	}

	int layer_counter = poDS->GetLayerCount();
	std::vector<DM::ViewContainer*> datastream;
	for (int i = 0; i < layer_counter; i++) {
		OGRLayer * lyr = poDS->GetLayer(i);
		DM::ViewContainer * v = viewContainerFactory(lyr);
		if (v) {
			datastream.push_back(v);
		}
	}
	this->registerViewContainers(datastream);

	OGRDataSource::DestroyDataSource(poDS);
}


void GDALHotStarter::run()
{
	DM::GDALSystem * sys = this->getGDALData("city");

	sys->setGDALDatabase(this->hotStartDatabase);
}
