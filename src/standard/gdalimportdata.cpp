#include "gdalimportdata.h"
#include "dmgdalsystem.h"
#include "ogr_geometry.h"

#include <algorithm>
#include <string>

#include "gdalutilities.h"
#include <ogr_api.h>

DM_DECLARE_NODE_NAME(GDALImportData, GDALModules)

GDALImportData::GDALImportData()
{
	GDALModule = true;
	OGRRegisterAll();
	this->addParameter("source", DM::FILENAME, &source);
	layername = "";
	this->addParameter("layer_name", DM::STRING, &layername);
	viewName = "";
	this->addParameter("view_name", DM::STRING, &viewName);
	append = false;
	this->addParameter("append", DM::BOOL, &append);

	this->epsg_to = 0;
	this->addParameter("epsg_to", DM::INT, &epsg_to);

	this->epsg_from = -1;
	this->addParameter("epsg_from", DM::INT, &epsg_from);

	//dummy to get the ports
	std::vector<DM::ViewContainer> data;
	data.push_back(  DM::ViewContainer ("dummy", DM::SUBSYSTEM, DM::WRITE) );
	this->addGDALData("city", data);

	vc = 0; //If still 0 after init() has been called somethig is wrong!
	poDS = 0;
	filterView = 0;
}

OGRCoordinateTransformation* GDALImportData::getTrafo(int sourceEPSG, int targetEPSG)
{
	if (sourceEPSG == targetEPSG)
		return 0;
	OGRSpatialReference* oSourceSRS;
	OGRSpatialReference* oTargetSRS;
	// GetSpatialRef: The returned object is owned by the OGRLayer and should not be modified or freed by the application.
	oSourceSRS = new OGRSpatialReference();
	oSourceSRS->importFromEPSG(sourceEPSG);

	oTargetSRS = new OGRSpatialReference();
	oTargetSRS->importFromEPSG(targetEPSG);
	// Input spatial reference system objects are assigned by copy
	// (calling clone() method) and no ownership transfer occurs.
	return OGRCreateCoordinateTransformation(oSourceSRS, oTargetSRS);
}

void GDALImportData::init()
{
	bool initFailed = false;
	if (layername.empty()) {
		DM::Logger(DM::Warning) << "no layer_name set";
		initFailed = true;
	}
	if (viewName.empty()) {
		DM::Logger(DM::Warning) << "no view_name set";
		initFailed = true;
	}
	if (source.empty()){
		DM::Logger(DM::Warning) << "no source set";
		initFailed = true;
	}

	if (initFailed) {

		std::vector<DM::ViewContainer> data;
		if (this->append)
			data.push_back(  DM::ViewContainer ("dummy", DM::SUBSYSTEM, DM::MODIFY) );
		else
			data.push_back(  DM::ViewContainer ("dummy", DM::SUBSYSTEM, DM::WRITE) );
		this->addGDALData("city", data);
		return;
	}


	vc = this->initShapefile();

	if (!vc)
		return;

	std::vector<DM::ViewContainer> views;
	views.push_back((*vc));

	if (append) {
		DM::ViewContainer dummy_view = DM::ViewContainer("dummy", DM::SUBSYSTEM, DM::MODIFY);
		views.push_back(dummy_view);
	}
	if (this->getFilter().size() > 0) {
		if (filterView)
			delete filterView;
		filterView = 0;
		foreach (DM::Filter f, this->getFilter()) {
			std::map<std::string, DM::View> inViews = getViewsInStream()["city"];
			if (inViews.find(f.getSpatialFilter().getArgument()) != inViews.end()) {
				DM::View v = inViews[f.getSpatialFilter().getArgument()];
				filterView = new DM::ViewContainer (v.getName(), v.getType(), DM::READ);
				views.push_back( (*filterView ));
			}
		}
	} else {
		filterView = 0;
	}
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
		this->setStatus(DM::MOD_EXECUTION_ERROR);
		return;
	}
	DM::GDALSystem * city = this->getGDALData("city");
	if (!city) {
		DM::Logger(DM::Error) << "No data stream returned";
		this->setStatus(DM::MOD_EXECUTION_ERROR);
		return;
	}
	OGRLayer * lyr = this->initLayer();
	if (!lyr) {
		DM::Logger(DM::Error) << "Init layer failed";
		this->setStatus(DM::MOD_EXECUTION_ERROR);
		return;
	}

	OGRFeature *poFeature;
	OGRMultiPolygon spatialFilter;
	vc->setCurrentGDALSystem(city);

	lyr->ResetReading();

	int epsg_from_internal = this->epsg_from;
	if (this->epsg_from == -1) {
		epsg_from_internal = lyr->GetSpatialRef()->GetEPSGGeogCS();
		DM::Logger(DM::Standard) << "Coordinate sytem identified as " << epsg_from_internal;
	}

	OGRCoordinateTransformation* forward_trans = this->getTrafo(epsg_from_internal, this->epsg_to);
	OGRCoordinateTransformation* backwards_trans = this->getTrafo(this->epsg_to, epsg_from_internal);

	if (!forward_trans || !backwards_trans) {
		DM::Logger(DM::Warning) << "Unknown Transformation";
	}

	//Add Spatial Filter
	if (filterView) {
		filterView->setCurrentGDALSystem(city);
		filterView->resetReading();
		while ( (poFeature = filterView->getNextFeature()) != NULL ) {
			OGRGeometry *ogr_t = poFeature->GetGeometryRef();
			if (backwards_trans)
				ogr_t->transform(backwards_trans);
			spatialFilter.addGeometry(ogr_t);
		}
		lyr->SetSpatialFilter(&spatialFilter);
	}
	//Add Attribute Filter
	if (this->getFilter().size() > 0) {
		foreach (DM::Filter f, this->getFilter()) {
			std::string filter = f.getAttributeFilter().getArgument();
			lyr->SetAttributeFilter(filter.c_str());
		}
	}

	int counter = 0;
	while( (poFeature = lyr->GetNextFeature()) != NULL ) {
		OGRGeometry * geo_single = 0;
		std::vector<OGRGeometry*> geo_collection;
		if (vc->getType() != DM::COMPONENT) {
			counter++;
			if (counter%100000 == 0){
				vc->syncAlteredFeatures();
			}
			OGRGeometry * geo_ref = poFeature->GetGeometryRef();
			if (geo_ref == 0) {
				DM::Logger(DM::Warning) << "Feature "<< poFeature->GetFID() << "not importet, no geometry";
				continue;
			}
			if (!geo_ref->IsValid()) {
				OGRGeometry * buf =  geo_ref->Buffer(0);
				if (!buf->IsValid()) {
					DM::Logger(DM::Warning) << "Feature "<< poFeature->GetFID() << "not importet, geometry is not valid";
					continue;
				} else {
					DM::Logger(DM::Warning) << "Try to use buffered feature for "<< poFeature->GetFID();
					geo_ref = buf;
				}
			}
			if (!this->checkIsFlat(geo_ref->getGeometryType())) {
				if (vc->getType() == DM::FACE) {
					OGRMultiPolygon * geo = (OGRMultiPolygon*) geo_ref;
					if (!geo)
						continue;
					if (geo->getNumGeometries() == 0)
						continue;
					geo_single = geo->getGeometryRef(0);//OGRGeometryFactory::forceToPolygon(geo);
					for (int i = 0; i < geo->getNumGeometries(); i++) {
						geo_collection.push_back(geo->getGeometryRef(i));
					}

				}
				if (vc->getType() == DM::EDGE) {
					OGRMultiLineString * geo = (OGRMultiLineString*) geo_ref;
					if (!geo)
						continue;
					if (geo->getNumGeometries() == 0)
						continue;
					geo_single = geo->getGeometryRef(0);
					for (int i = 0; i < geo->getNumGeometries(); i++) {
						geo_collection.push_back(geo->getGeometryRef(i));
					}
				}
				if (vc->getType() == DM::NODE) {
					OGRMultiPoint * geo = (OGRMultiPoint*) geo_ref;
					if (!geo)
						continue;
					if (geo->getNumGeometries() == 0)
						continue;
					geo_single = geo->getGeometryRef(0);
					for (int i = 0; i < geo->getNumGeometries(); i++) {
						geo_collection.push_back(geo->getGeometryRef(i));
					}
				}
			} else {
				geo_single = poFeature->GetGeometryRef();
				geo_collection.push_back(geo_single);
			}
			//Check Type is fine
			if (wkbFlatten(geo_single->getGeometryType()) != DM::GDALUtilities::DMToOGRGeometry(vc->getType())) {
				DM::Logger(DM::Warning) << "Feature "
										<< poFeature->GetFID()
										<< " not importet, expected "
										<< (int) DM::GDALUtilities::DMToOGRGeometry(vc->getType())
										<< " instead of " << (int) wkbFlatten(geo_single->getGeometryType())
										<< " geometry type is different";
				continue;
			}

		} else {
			geo_collection.push_back(0); // Add empty element to add geometry
		}

		foreach(OGRGeometry * g, geo_collection){
			OGRFeature * f_new = vc->createFeature();
				if(g) {
					if (forward_trans)
						g->transform(forward_trans);
					g->flattenTo2D();
					f_new->SetGeometry(g);
				}
				foreach(std::string attribute_name, vc->getAllAttributes()) {
					std::string real_name = translator[attribute_name];
					OGRField * f = poFeature->GetRawFieldRef(poFeature->GetFieldIndex(real_name.c_str()));
					f_new->SetField(attribute_name.c_str(), f);
				}
		}
		OGRFeature::DestroyFeature( poFeature );
	}
	vc->syncAlteredFeatures();
	if (filterView)
		filterView->syncReadFeatures();
}

DM::ViewContainer *GDALImportData::initShapefile()
{
	if (vc) { // if vc is set delete and reset
		delete vc;
		vc = 0;
	}
	DM::Logger(DM::Debug) << "Init Shapefile";
	//Create View
	OGRLayer * lyr = initLayer();

	if (!lyr) {
		DM::Logger(DM::Error) << "Init failed";
		return 0;
	}

	OGRFeatureDefn * def = lyr->GetLayerDefn();

	int dm_geometry = DM::GDALUtilities::OGRtoDMGeometry(def);
	translator.clear();
	DM::ViewContainer * view = new DM::ViewContainer(this->viewName, dm_geometry, DM::WRITE);
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

OGRLayer *GDALImportData::initLayer()
{
	if (poDS) {
		OGRDataSource::DestroyDataSource(poDS);
		poDS = 0;
	}
	OGRDataSource *poDS = OGRSFDriverRegistrar::Open(this->source.c_str());
	if (!poDS) {
		DM::Logger(DM::Warning) << "Error loading " << this->source;
		return 0;
	}
	OGRLayer * lyr = poDS->GetLayerByName(this->layername.c_str());
	if (!lyr) {
		DM::Logger(DM::Warning) << "Error loading " << this->layername;
		return 0;
	}
	return lyr;
}



bool GDALImportData::checkIsFlat(int ogrType)
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

string GDALImportData::getHelpUrl()
{
	return "https://github.com/iut-ibk/DynaMind-BasicModules/blob/master/doc/gdalimportdata.rsthttps://github.com/christianurich/DynaMind-GDALModules/blob/master/doc/gdalimportdata.rst";
}




