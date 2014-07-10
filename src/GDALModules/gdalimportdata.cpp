#include "gdalimportdata.h"
#include "dmgdalsystem.h"


#include <ogr_api.h>

DM_DECLARE_NODE_NAME(GDALImportData, GDALModules)



GDALImportData::GDALImportData()
{
	GDALModule = true;
	OGRRegisterAll();
	driverType = "";
	this->addParameter("driver_type", DM::STRING, &driverType);
	source = "";
	this->addParameter("source", DM::FILENAME, &source);
	layername = "";
	this->addParameter("layer_name", DM::STRING, &layername);
	viewName = "";
	this->addParameter("view_name", DM::STRING, &viewName);

	vc = 0; //If still 0 after init() has been called somethig is wrong!
	poDS = 0;
}

void GDALImportData::init()
{

	if (driverType.empty()) {
		DM::Logger(DM::Error) << "no driver_type set";
		return;
	}
	if (layername.empty()) {
		DM::Logger(DM::Error) << "no layer_name set";
		return;
	}
	if (viewName.empty()) {
		DM::Logger(DM::Error) << "no view_name set";
		return;
	}
	if (source.empty()){
		DM::Logger(DM::Error) << "no source set";
		return;
	}

	vc = this->initShapefile();

	if (!vc)
		return;

	std::vector<DM::ViewContainer> views;
	views.push_back((*vc));

	this->addGDALData("city", views);

}

GDALImportData::~GDALImportData()
{
	if (vc) { // if vc is set delete and reset
		delete vc;
	}
	if (poDS) {
		OGRDataSource::DestroyDataSource(poDS);
	}
}


void GDALImportData::run()
{
	DM::Logger(DM::Debug) << "Run GDALImportData";
	if (!vc) {
		DM::Logger(DM::Error) << "Init view failed";
		return;
	}
	DM::GDALSystem * city = this->getGDALData("city");
	if (!city) {
		DM::Logger(DM::Error) << "No data stream returned";
		return;
	}
	city->updateView(*vc);
	vc->setCurrentGDALSystem(city);

	OGRLayer * lyr = this->initLayer();
	lyr->ResetReading();
	OGRFeature *poFeature;

	while( (poFeature = lyr->GetNextFeature()) != NULL ) {
		OGRFeature * f_new = vc->createFeature();
		f_new->SetGeometry(poFeature->GetGeometryRef());
		foreach(std::string attribute_name, vc->getAllAttributes()) {
			OGRField * f = poFeature->GetRawFieldRef(poFeature->GetFieldIndex(attribute_name.c_str()));
			f_new->SetField(attribute_name.c_str(), f);
		}
		OGRFeature::DestroyFeature( poFeature );
	}
	vc->syncAlteredFeatures();
}

DM::ViewContainer * GDALImportData::initShapefile()
{
	if (vc) { // if vc is set delete and reset
		delete vc;
		vc = 0;
	}
	DM::Logger(DM::Debug) << "Init Shapefile";
	//Create View
	OGRLayer * lyr = initLayer();
	OGRFeatureDefn * def = lyr->GetLayerDefn();

	int dm_geometry = this->OGRtoDMGeometry(def);

	DM::ViewContainer * view = new DM::ViewContainer(this->viewName, dm_geometry, DM::WRITE);
	for (int i = 0; i < def->GetFieldCount(); i++){
		OGRFieldDefn * fdef = def->GetFieldDefn(i);
		//DM Datatype
		DM::Attribute::AttributeType type = OGRToDMAttribute(fdef);
		if (type == DM::Attribute::NOTYPE)
			continue;
		DM::Logger(DM::Debug) << "Load attribute" << fdef->GetNameRef();
		view->addAttribute(fdef->GetNameRef(), type, DM::WRITE);
	}
	return view;
}

OGRLayer *GDALImportData::initLayer()
{
	if (poDS) {
		OGRDataSource::DestroyDataSource(poDS);
		poDS = 0;
	}
	OGRDataSource *poDS = OGRSFDriverRegistrar::Open(this->source.c_str());
	if (!poDS) {
		DM::Logger(DM::Error) << "Error loading " << this->source;
		return 0;
	}
	OGRLayer * lyr = poDS->GetLayerByName(this->layername.c_str());
	if (!lyr) {
		DM::Logger(DM::Error) << "Error loading " << this->layername;
		return 0;
	}
	return lyr;
}

DM::Attribute::AttributeType GDALImportData::OGRToDMAttribute(OGRFieldDefn * fdef) {
	DM::Attribute::AttributeType type = DM::Attribute::NOTYPE;
	switch (fdef->GetType()) {
	case OFTInteger:
		type = DM::Attribute::DOUBLE;
		break;
	case OFTReal:
		type = DM::Attribute::DOUBLE;
		break;
	case OFTString:
		type = DM::Attribute::STRING;
		break;
	default:
		DM::Logger(DM::Error) << "Type not supported attributed " <<fdef->GetNameRef() << " not loaded";
		break;
	}
	return type;
}

int GDALImportData::OGRtoDMGeometry(OGRFeatureDefn *def)
{
	int type = -1;
	OGRwkbGeometryType ogrType = def->GetGeomType();
	std::string strType = OGRGeometryTypeToName(ogrType);
	switch(wkbFlatten(ogrType))
	{
	case wkbPoint:
		type = DM::NODE;
		break;
	case wkbPolygon:
		type = DM::FACE;
		break;
	case wkbMultiPolygon:
		type = DM::FACE;
		break;
	case wkbLineString:
		type = DM::EDGE;
		break;
	case wkbMultiLineString:
		type = DM::EDGE;
		break;
	default:
		DM::Logger(DM::Error) << "Geometry type not implemented: " << strType;
		return -1;
	}
	DM::Logger(DM::Debug) << "Found: Geometry type " << strType;
	return type;
}


