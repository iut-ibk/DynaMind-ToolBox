#include "gdalhotstarter.h"

#include <ogrsf_frmts.h>
#include "gdalutilities.h"
#include <dmgdalsystem.h>
#include <sstream>


DM_DECLARE_CUSTOM_NODE_NAME(GDALHotStarter, Load Simulation DB , Data Import and Export)

DM::ViewContainer *GDALHotStarter::viewContainerFactory(OGRLayer *lyr, OGRLayer *def_lyr)
{
	OGRFeatureDefn * def = lyr->GetLayerDefn();
	int dm_geometry = DM::GDALUtilities::OGRtoDMGeometry(def);
	std::map<std::string, std::string> translator;
	DM::ViewContainer * view = new DM::ViewContainer(lyr->GetName(), dm_geometry, DM::WRITE);
	std::stringstream attribute_filter;
	attribute_filter << "view_name = '";
	attribute_filter << lyr->GetName();
	attribute_filter << "'";
	def_lyr->SetAttributeFilter(attribute_filter.str().c_str());
	def_lyr->ResetReading();
	OGRFeature * f;

	while (f = def_lyr->GetNextFeature()) {
		DM::Logger(DM::Debug) << f->GetFieldAsString("attribute_name");
		std::string attribute_name = QString::fromStdString(f->GetFieldAsString("attribute_name")).toStdString();
		if (attribute_name == "DEFINITION")
			continue;
		std::string data_type = QString::fromStdString(f->GetFieldAsString("data_type")).toStdString();
		view->addAttribute(attribute_name.c_str(), DM::GDALUtilities::AttributeTypeStringToType(data_type), DM::WRITE);
		OGRFeature::DestroyFeature(f);
		//def_lyr->GetNextFeature();
	}

	/*for (int i = 0; i < def->GetFieldCount(); i++){
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
	}*/
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
	if (this->hotStartDatabase.empty()) {
		DM::Logger(DM::Warning) << "No database set";
		return;
	}
	OGRDataSource *poDS = OGRSFDriverRegistrar::Open(this->hotStartDatabase.c_str());
	if (!poDS) {
		DM::Logger(DM::Warning) << "Error loading database: " << this->hotStartDatabase;
		return;
	}
	int layer_counter = poDS->GetLayerCount();
	std::vector<DM::ViewContainer*> datastream;
	OGRLayer * layer_def = poDS->GetLayerByName("dynamind_table_definitions");
	for (int i = 0; i < layer_counter; i++) {
		OGRLayer * lyr = poDS->GetLayer(i);
		if (QString::fromStdString(lyr->GetName()) == "dynamind_table_definitions")
			continue;
		DM::ViewContainer * v = viewContainerFactory(lyr, layer_def);
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

string GDALHotStarter::getHelpUrl()
{
	return "/DynaMind-GDALModules/gdalhotstarter.html";
}

