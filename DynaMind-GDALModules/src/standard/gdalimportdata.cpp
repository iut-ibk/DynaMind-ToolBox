#include "gdalimportdata.h"
#include "dmgdalsystem.h"
#include "ogr_geometry.h"
#include "dmsimulation.h"

#include <algorithm>
#include <string>

#include "gdalutilities.h"
#include <ogr_api.h>



DM_DECLARE_CUSTOM_NODE_NAME(GDALImportData, Import GIS Data , Data Import and Export)

GDALImportData::GDALImportData()
{
	GDALModule = true;
	OGRRegisterAll();
	this->addParameter("source", DM::FILENAME, &source);

	layername = "";
	this->addParameter("layer_name", DM::STRING, &layername);

	viewName = "";
	this->addParameter("view_name", DM::STRING, &viewName);

	import_attribute_as.clear();
	this->addParameter("import_attribute_as", DM::STRING_MAP, &import_attribute_as);

	append = false;
	this->addParameter("append", DM::BOOL, &append);

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

	if (!vc) {
		if (poDS) {
			OGRDataSource::DestroyDataSource(poDS);
			poDS = 0;
		}
		return;
	}

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
	if (poDS) {
		OGRDataSource::DestroyDataSource(poDS);
		poDS = 0;
	}
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


	OGRCoordinateTransformation* forward_trans = 0;
	OGRCoordinateTransformation* backwards_trans = 0;

	if (vc->getType() != DM::COMPONENT) {
		forward_trans = this->getTrafo(epsg_from_internal, this->getSimulation()->getSimulationConfig().getCoorindateSystem());
		backwards_trans = this->getTrafo(this->getSimulation()->getSimulationConfig().getCoorindateSystem(), epsg_from_internal);
	}

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
			if (!geo_ref) {
				DM::Logger(DM::Warning) << "Feature "<< poFeature->GetFID() << "not imported, no geometry";
				continue;
			}
			if (!geo_ref->IsValid()) {
				OGRGeometry * buf =  geo_ref->Buffer(0);
				if (!buf) {
					DM::Logger(DM::Warning) << "Feature "<< poFeature->GetFID() << "not imported, geometry buffering failed";
					continue;
				}
				if (!buf->IsValid()) {
					DM::Logger(DM::Warning) << "Feature "<< poFeature->GetFID() << "not imported, geometry is not valid";
					continue;
				}
				DM::Logger(DM::Warning) << "Try to use buffered feature for "<< poFeature->GetFID();
				geo_ref = buf;

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
										<< " not imported, expected "
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
	if (poDS) {
		OGRDataSource::DestroyDataSource(poDS);
		poDS = 0;
	}
}


DM::ViewContainer *GDALImportData::initShapefile() // Init view container
{
	if (vc) { // if vc is set delete and reset
		delete vc;
		vc = 0;
	}
	DM::Logger(DM::Debug) << "Init Shapefile";
	//Create View
	OGRLayer * lyr = initLayer();

	if (!lyr) {
		DM::Logger(DM::Error) << "Failed to init gis layer";
		return 0;
	}

	OGRFeatureDefn * def = lyr->GetLayerDefn();

	if (this->epsg_from == -1 && this->driver_name != "CSV") {
		OGRSpatialReference * sr  = lyr->GetSpatialRef();
		if (sr)
			this->epsg_from = sr->GetEPSGGeogCS();
		DM::Logger(DM::Standard) << "Coordinate sytem identified as " << this->epsg_from;
	}

	int dm_geometry = DM::GDALUtilities::OGRtoDMGeometry(def);
	translator.clear();
	DM::ViewContainer * view = new DM::ViewContainer(this->viewName, dm_geometry, DM::WRITE);
	if (import_attribute_as.empty()) { //If no attribtue set import everthing
		for (int i = 0; i < def->GetFieldCount(); i++){
			OGRFieldDefn * fdef = def->GetFieldDefn(i);
			//DM Datatype
			DM::Attribute::AttributeType type = DM::GDALUtilities::OGRToDMAttribute(fdef);
			if (type == DM::Attribute::NOTYPE)
				continue;
			DM::Logger(DM::Debug) << "Load attribute" << fdef->GetNameRef();

			//everthing needs to be lower case
			std::string attribute_name = fdef->GetNameRef();
			std::transform(attribute_name.begin(), attribute_name.end(), attribute_name.begin(), ::tolower);

			if (attribute_name == "ogc_fid") //Don't import ogc_fid field. This may cause a problem
				continue;
			translator[attribute_name] = fdef->GetNameRef();
			view->addAttribute(attribute_name.c_str(), type, DM::WRITE);
		}
	} else { //if attribute is set stick to list
		for (std::map<std::string, std::string>::const_iterator it = import_attribute_as.begin();
			 it != import_attribute_as.end(); ++it) {
				int f_index = def->GetFieldIndex(it->first.c_str());
				if(f_index < 0) {
					DM::Logger(DM::Error) << "Attribute " << it->first << " not found";
					delete view;
					return 0;
				}
				OGRFieldDefn * fdef = def->GetFieldDefn(f_index);
				DM::Attribute::AttributeType type = DM::GDALUtilities::OGRToDMAttribute(fdef);
				if (type == DM::Attribute::NOTYPE) {
					DM::Logger(DM::Error) << "Attribute " << it->first << " unknown type";
					delete view;
					return 0;
				}
				std::string attribute_name = it->second;
				translator[attribute_name] = it->first;
				view->addAttribute(attribute_name.c_str(), type, DM::WRITE);
		}
	}
	return view;
}

OGRLayer *GDALImportData::initLayer()
{
	if (poDS) {
		OGRDataSource::DestroyDataSource(poDS);
		poDS = 0;
	}
	poDS = OGRSFDriverRegistrar::Open(this->source.c_str());
	if (!poDS) {
		DM::Logger(DM::Warning) << "Error loading " << this->source;
		return 0;
	}
	this->driver_name = poDS->GetDriver()->GetName();
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
	return "/DynaMind-GDALModules/gdalimportdata.html";
}




