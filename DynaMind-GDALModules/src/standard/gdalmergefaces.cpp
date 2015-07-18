#include "gdalmergefaces.h"


#include <dmviewcontainer.h>
#include <dmview.h>
#include <dmgdalsystem.h>
#include <dm.h>
#include <sstream>
#include <ogrsf_frmts.h>

DM_DECLARE_CUSTOM_NODE_NAME(GDALMergeFaces,Merge Faces, Geometry Processing)

GDALMergeFaces::GDALMergeFaces()
{
	GDALModule = true;


	this->viewName = "";
	this->addParameter("view_name", DM::STRING, &viewName);

	this->combinedViewName = "";
	this->addParameter("combined_view_name", DM::STRING, &combinedViewName);

	this->attriubteName = "";
	this->addParameter("attribute_name", DM::STRING, &attriubteName);

	//dummy to get the ports
	std::vector<DM::ViewContainer> data;
	data.push_back(  DM::ViewContainer ("dummy", DM::SUBSYSTEM, DM::MODIFY) );
	this->addGDALData("city", data);
}

void GDALMergeFaces::init()
{
	if (this->viewName.empty() || this->combinedViewName.empty()) {
		return;
	}

	std::map<std::string, DM::View> inViews = getViewsInStream()["city"];
	if (inViews.find(viewName) == inViews.end()) {
		DM::Logger(DM::Warning) << "view " << viewName << " not found in in stream";
		return;
	}

	leadingView = DM::ViewContainer(this->viewName, DM::FACE, DM::READ);
	leadingView.addAttribute(attriubteName, DM::Attribute::INT, DM::READ);

	combinedView = DM::ViewContainer(this->combinedViewName, DM::FACE, DM::WRITE);
	combinedView.addAttribute("test_id", DM::Attribute::INT, DM::WRITE);

	std::vector<DM::ViewContainer*> data;
	data.push_back( &leadingView);
	data.push_back( &combinedView);
	this->registerViewContainers(data);
}

OGRGeometry *  GDALMergeFaces::joinCluster(int cluster) {
	std::stringstream query;
	query << attriubteName << " = " << cluster;


	leadingView.setAttributeFilter(query.str().c_str());
	leadingView.resetReading();

	OGRFeature * f;
	OGRGeometry * geo = 0;

	while (f = leadingView.getNextFeature()) {
		if (!geo) {
			geo = f->GetGeometryRef();
			continue;
		}
		geo = geo->Union(f->GetGeometryRef());
		if (!geo)
			DM::Logger(DM::Error) << "Something when wrong when unioning";
	}

	return geo;
}

void GDALMergeFaces::run()
{
	OGRGeometry * geo;
	leadingView.resetReading();
	int cluster_id = 1;
	int counter = 1;
	while(geo = joinCluster(cluster_id)) {
		cluster_id++;
		counter++;
		if (wkbMultiPolygon == geo->getGeometryType()){
			geo = geo->UnionCascaded();
			OGRMultiPolygon * mgeo = (OGRMultiPolygon*) geo;
			if (mgeo->getNumGeometries() == 0) {
				continue;
			}
			geo = mgeo->getGeometryRef(0);

			int n = mgeo->getNumGeometries();
			for (int i = 0; i < n; i++) {
				OGRFeature * f = combinedView.createFeature();

				f->SetGeometry(mgeo->getGeometryRef(i));
				f->SetField("test_id", counter);
			}
			continue;
		}
		OGRFeature * f = combinedView.createFeature();
		f->SetGeometry(geo);
	}

}
