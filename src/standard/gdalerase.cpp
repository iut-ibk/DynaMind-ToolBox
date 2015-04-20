#include "gdalerase.h"
#include <ogr_feature.h>

DM_DECLARE_NODE_NAME(GDALErase, GDALModules)

GDALErase::GDALErase()
{
	this->GDALModule = true;

	this->leadingViewName = "";
	this->addParameter("leadingViewName", DM::STRING, &leadingViewName);


	this->eraseViewName = "";
	this->addParameter("eraseViewName", DM::STRING, &eraseViewName);

	//dummy to get the ports
	std::vector<DM::ViewContainer> data;
	data.push_back(  DM::ViewContainer ("dummy", DM::SUBSYSTEM, DM::MODIFY) );
	this->addGDALData("city", data);
}

void GDALErase::init()
{
	if (leadingViewName.empty() || eraseViewName.empty()){
		return;
	}

	this->leadingView = DM::ViewContainer(this->leadingViewName, DM::FACE, DM::MODIFY);
	this->eraseView = DM::ViewContainer(this->eraseViewName, DM::FACE, DM::READ);
	std::vector<DM::ViewContainer * > data_stream;
	data_stream.push_back(&leadingView);
	data_stream.push_back(&eraseView);

	this->registerViewContainers(data_stream);
}

void GDALErase::run()
{
	eraseView.resetReading();

	OGRFeature * erase = 0;
	//Effected Features
	//std::map<long, OGRFeature*> effected_features;
	std::vector<OGRFeature*> erase_features;
	std::map<OGRFeature*, std::vector<long> > intersections;
	std::map<long, OGRGeometry*> geometries;
	std::map<long, OGRFeature*> existing_features;
	while (erase = eraseView.getNextFeature()) {
		std::vector<long> intersects;

		leadingView.setSpatialFilter(erase->GetGeometryRef());
		leadingView.resetReading();
		OGRFeature * feat_existing = 0;
		while (feat_existing = leadingView.getNextFeature()) {
			intersects.push_back(feat_existing->GetFID());
			geometries[feat_existing->GetFID()] = feat_existing->GetGeometryRef();
			existing_features[feat_existing->GetFID()] = feat_existing;
		}
		if (intersects.size() > 0) {
			intersections[erase] = intersects;
		}
		erase_features.push_back(erase);
	}
	//Intersect

	foreach(OGRFeature * er, erase_features){

		OGRGeometry * geo_erase = er->GetGeometryRef();

		std::vector<long> effected = intersections[er];

		foreach(long existing, effected) {

			OGRGeometry * geo_existing = geometries[existing];
			if (!geo_existing)
				continue;
			OGRGeometry * result = geo_existing->Difference(geo_erase);

			if (result->IsEmpty()){
				leadingView.deleteFeature(existing);
				geometries[existing]  = 0;
				continue;
			}
			if (!result->IsValid()){
				leadingView.deleteFeature(existing);
				geometries[existing]  = 0;
				continue;
			}
			if (result->getGeometryType() != wkbPolygon) {
				leadingView.deleteFeature(existing);
				geometries[existing] = 0;
				continue;
			}
			OGRPolygon * poly = (OGRPolygon*) result;
			if ( poly->get_Area()  < 10.) {
				leadingView.deleteFeature(existing);
				geometries[existing]  = 0;
				continue;
			}
			geometries[existing]  = result;
			existing_features[existing]->SetGeometry(result);
		}
	}

}



