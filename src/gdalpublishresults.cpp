#include "gdalpublishresults.h"

#include "ogr_geometry.h"


#include <ogrsf_frmts.h>
#include <ogr_api.h>

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

	DM::ViewContainer v("dummy", DM::SUBSYSTEM, DM::READ);


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

	std::vector<DM::ViewContainer *> data_stream;

	data_stream.push_back(&components);
	this->registerViewContainers(data_stream);
}


void GDALPublishResults::run()
{
	//Init Data Source
	OGRSFDriver *poDriver;
	OGRDataSource *poDS;

	OGRRegisterAll();

	if (!driverName.empty()) {
		poDriver  = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName( this->driverName.c_str() );
		if( poDriver == NULL )
		{
			DM::Logger(DM::Error) << this->driverName << "driver not available.";
			this->setStatus(DM::MOD_EXECUTION_ERROR);
			return;
		}

		poDS = poDriver->CreateDataSource(this->sink.c_str());
	} else {
		poDS = OGRSFDriverRegistrar::Open(this->sink.c_str(),true);
		//poDS = (this->sink.c_str());
	}
	if( poDS == NULL )
	{
		DM::Logger(DM::Error) <<  "problem creating source";
		this->setStatus(DM::MOD_EXECUTION_ERROR);
		return;
	}

	OGRSpatialReference* oSourceSRS;
	oSourceSRS = new OGRSpatialReference();
	oSourceSRS->importFromEPSG(4283);

	OGRLayer * lyr;
	switch ( components.getType() ) {
	case DM::COMPONENT:
		lyr = poDS->CreateLayer(this->layerName.c_str(), oSourceSRS, wkbNone, NULL );
		break;
	case DM::NODE:
		lyr = poDS->CreateLayer(this->layerName.c_str(), oSourceSRS, wkbPoint, NULL );
		break;
	case DM::EDGE:
		lyr = poDS->CreateLayer(this->layerName.c_str(), oSourceSRS, wkbLineString, NULL );
		break;
	case DM::FACE:
		lyr = poDS->CreateLayer(this->layerName.c_str(), oSourceSRS, wkbPolygon, NULL );
		break;
	}
	if (!lyr) {
		DM::Logger(DM::Error) << "Layer not created";
		this->setStatus(DM::MOD_EXECUTION_ERROR);
		return;
	}
	OGRFeatureDefn * def =  (OGRFeatureDefn*)components.getFeatureDef();
	int c_fields = def->GetFieldCount();
	for (int i = 0; i < c_fields; i++)
		lyr->CreateField(def->GetFieldDefn(i));
	OGRFeature * feat;
	components.resetReading();
	std::vector<OGRFeature*> features_write;
	int counter = 0;
	while (feat = components.getNextFeature()) {
		counter++;
		if (counter % 100000 == 0){
			writeFeatures(lyr, features_write);
			components.syncReadFeatures();
		}
		OGRFeature * f_new = OGRFeature::CreateFeature( lyr->GetLayerDefn() );
		for (int i = 0; i < c_fields; i++)
			f_new->SetField(i, feat->GetRawFieldRef(i));
		f_new->SetGeometry(feat->GetGeometryRef());
		features_write.push_back(f_new);
	}
	writeFeatures(lyr, features_write);
	OGRDataSource::DestroyDataSource(poDS);
}

void GDALPublishResults::writeFeatures(OGRLayer * lyr, std::vector<OGRFeature *> & feats) {
	lyr->StartTransaction();
	foreach (OGRFeature * f, feats ) {
		lyr->CreateFeature(f);
		OGRFeature::DestroyFeature(f);
	}
	lyr->CommitTransaction();
	feats.clear();
}

