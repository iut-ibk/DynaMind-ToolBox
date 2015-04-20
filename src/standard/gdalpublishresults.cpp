#include "gdalpublishresults.h"

#include "ogr_geometry.h"


#include <ogrsf_frmts.h>
#include <ogr_api.h>


#include <dmgroup.h>
#include <sstream>

DM_DECLARE_NODE_NAME(GDALPublishResults, GDALModules)

GDALPublishResults::GDALPublishResults()
{
	GDALModule = true;
	OGRRegisterAll();

	this->sink = "";
	this->addParameter("sink", DM::STRING, &sink);

	this->driverName = "";
	this->addParameter("driver_name", DM::STRING, &driverName);
	this->viewName = "";
	this->addParameter("view_name", DM::STRING, &viewName);

	this->layerName = "";
	this->addParameter("layer_name", DM::STRING, &layerName);

	this->steps = 1;
	this->addParameter("steps", DM::INT, &steps);

	this->targetEPSG = 0;
	this->addParameter("targetEPSG", DM::INT, &targetEPSG);

	this->sourceEPSG = 0;
	this->addParameter("sourceEPSG", DM::INT, &sourceEPSG);

	this->overwrite = false;
	this->addParameter("overwrite", DM::BOOL, &overwrite);

	DM::ViewContainer v("dummy", DM::SUBSYSTEM, DM::MODIFY);


	std::vector<DM::ViewContainer> datastream;
	datastream.push_back(v);
	this->addGDALData("city", datastream);
}

void GDALPublishResults::init()
{
	if (this->viewName.empty()) {
		return;
	}

	std::map<std::string, DM::View> inViews = getViewsInStream()["city"];
	if (inViews.find(viewName) == inViews.end()) {
		DM::Logger(DM::Error) << "view " << viewName << " not found in in stream";
		return;
	}
	this->components = DM::ViewContainer(viewName, inViews[viewName].getType(), DM::READ);
	this->dummy = DM::ViewContainer(viewName, inViews[viewName].getType(), DM::MODIFY);

	std::vector<DM::ViewContainer *> data_stream;

	data_stream.push_back(&components);
	data_stream.push_back(&dummy);
	this->registerViewContainers(data_stream);
}

void GDALPublishResults::run()
{
	//Init Data Source
	OGRSFDriver *poDriver;
	OGRDataSource *poDS;

	DM::Group* lg = dynamic_cast<DM::Group*>(getOwner());
	int interal_counter = -1;
	if(lg) {
		interal_counter = lg->getGroupCounter();
	}
	DM::Logger(DM::Debug) << interal_counter;

	if (interal_counter % this->steps != 0 && interal_counter != -1) {
		return;
	}
	DM::Logger(DM::Debug) << "Start Inster";

	char ** options = NULL;
	options = CSLSetNameValue( options, "PG_USE_COPY", "YES" );

	//Set Overwrite
	if (overwrite)
		options = CSLSetNameValue( options, "OVERWRITE", "YES" );

	CPLSetConfigOption("PG_USE_COPY", "YES");
	OGRRegisterAll();

	//Extend string_stream with _xx;
	std::stringstream sink_name;
	if (!driverName.empty()) {
		int pos = this->sink.find(".");
		if (pos == -1) {
			sink_name << sink;
			sink_name << "_" << interal_counter;
		} else {
			sink_name<< sink.substr(0,pos);
			sink_name << "_" << interal_counter;
			sink_name<< sink.substr(pos, sink.size()-1);
		}
	}

	if (!driverName.empty()) {
		poDriver  = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName( this->driverName.c_str() );
		if( poDriver == NULL )
		{
			DM::Logger(DM::Error) << this->driverName << "driver not available.";
			this->setStatus(DM::MOD_EXECUTION_ERROR);
			return;
		}
		DM::Logger(DM::Error) << "create " << sink_name.str().c_str();
		poDS = poDriver->CreateDataSource(sink_name.str().c_str());
	} else {
		poDS = OGRSFDriverRegistrar::Open(sink.c_str(), true);
	}
	if( poDS == NULL )
	{
		DM::Logger(DM::Error) <<  "problem creating source";
		this->setStatus(DM::MOD_EXECUTION_ERROR);
		return;
	}


	OGRSpatialReference* oSourceSRS;
	OGRSpatialReference* oTargetSRS;
	oSourceSRS = new OGRSpatialReference();
	oSourceSRS->importFromEPSG(sourceEPSG);

	oTargetSRS = new OGRSpatialReference();
	oTargetSRS->importFromEPSG(targetEPSG);
	OGRCoordinateTransformation* trans = OGRCreateCoordinateTransformation(oSourceSRS, oTargetSRS);
	if (!trans) {
		DM::Logger(DM::Error) << "Init transformation failed, check EPSG codes";
		return;
	}
	std::stringstream layer_name;
	layer_name << this->layerName;
	if (interal_counter != -1) {
		 layer_name << "_" << interal_counter;
	}

	OGRLayer * lyr;

	switch ( components.getType() ) {
	case DM::COMPONENT:
		lyr = poDS->CreateLayer(layer_name.str().c_str(), oTargetSRS, wkbNone, options );
		break;
	case DM::NODE:
		lyr = poDS->CreateLayer(layer_name.str().c_str(), oTargetSRS, wkbPoint, options );
		break;
	case DM::EDGE:
		lyr = poDS->CreateLayer(layer_name.str().c_str(), oTargetSRS, wkbLineString, options );
		break;
	case DM::FACE:
		lyr = poDS->CreateLayer(layer_name.str().c_str(), oTargetSRS, wkbPolygon, options );
		break;
	}

	if (!lyr) {
		DM::Logger(DM::Error) << "Layer not created " << components.getType();
		this->setStatus(DM::MOD_EXECUTION_ERROR);
		return;
	}

	OGRFeatureDefn * def =  (OGRFeatureDefn*)components.getFeatureDef();
	int c_fields = def->GetFieldCount();
	for (int i = 0; i < c_fields; i++)
		lyr->CreateField(def->GetFieldDefn(i));
	OGRFeature * feat;
	components.resetReading();
	int counter = 0;
	lyr->StartTransaction();
	while (feat = components.getNextFeature()) {
		counter++;
		if (counter % 1000000 == 0){
			lyr->CommitTransaction();
			lyr->StartTransaction();
		}
		OGRFeature * f_new = OGRFeature::CreateFeature( lyr->GetLayerDefn() );
		for (int i = 0; i < c_fields; i++)
			f_new->SetField(i, feat->GetRawFieldRef(i));
		if (components.getType() != DM::COMPONENT) {
			OGRGeometry * geo = feat->GetGeometryRef();
			if (trans)
				geo->transform(trans);
			f_new->SetGeometry(geo);
		}
		lyr->CreateFeature(f_new);
		OGRFeature::DestroyFeature(f_new);
	}
	lyr->CommitTransaction();
	OGRDataSource::DestroyDataSource(poDS);
}


