#include "gdalclusterneighbourhood.h"
#include <dmviewcontainer.h>
#include <dmview.h>
#include <dmgdalsystem.h>
#include <dm.h>
#include <sstream>
#include <ogrsf_frmts.h>
#include <map>


DM_DECLARE_NODE_NAME(GDALClusterNeighbourhood, GDALModules)

GDALClusterNeighbourhood::GDALClusterNeighbourhood()
{
	GDALModule = true;


	this->viewName = "";
	this->addParameter("view_name", DM::STRING, &viewName);

	this->neighName = "";
	this->addParameter("neigh_name", DM::STRING, &neighName);

	//dummy to get the ports
	std::vector<DM::ViewContainer> data;
	data.push_back(  DM::ViewContainer ("dummy", DM::SUBSYSTEM, DM::MODIFY) );
	this->addGDALData("city", data);
}

void GDALClusterNeighbourhood::init()
{
	if (this->viewName.empty() || this->neighName.empty()) {
		return;
	}

	std::map<std::string, DM::View> inViews = getViewsInStream()["city"];
	if (inViews.find(viewName) == inViews.end()) {
		DM::Logger(DM::Warning) << "view " << viewName << " not found in in stream";
		return;
	}

	name_id1 = this->viewName + "_id1";
	name_id2 = this->viewName + "_id2";

	leadingView = DM::ViewContainer(this->viewName, DM::FACE, DM::READ);
	leadingView.addAttribute("cluster_id", DM::Attribute::DOUBLE, DM::WRITE);

	ngView = DM::ViewContainer(this->neighName, DM::COMPONENT, DM::READ);

	ngView.addAttribute(name_id1, this->viewName, DM::READ);
	ngView.addAttribute(name_id2, this->viewName, DM::READ);

	std::vector<DM::ViewContainer*> data;
	data.push_back( &leadingView);
	data.push_back( &ngView);
	this->registerViewContainers(data);

}

void GDALClusterNeighbourhood::getNext(int id, int marker, std::map<int, int> & visited) {

	if (visited[id] > 0)
		return;

	visited[id] = marker;
	//Find my neighbours
	std::stringstream query;
	query << name_id1 << " = " << id;

	ngView.resetReading();
	ngView.setAttributeFilter(query.str().c_str());

	OGRFeature * ng;

	std::vector<int> neighbours;
	while (ng = ngView.getNextFeature()) {
		int id2 = ng->GetFieldAsInteger(name_id2.c_str());
		neighbours.push_back(id2);
	}

	foreach(int n, neighbours)
		getNext(n, marker, visited);
}


void GDALClusterNeighbourhood::run()
{
	ngView.createIndex(name_id1);

	leadingView.resetReading();
	OGRFeature * f;
	std::map<int, int> visited;
	int marker = 0;
	while (f = leadingView.getNextFeature()) {
		int id = f->GetFID();
		visited[id] = -1;
	}
	while (f = leadingView.getNextFeature()) {
		int id = f->GetFID();
		if (visited[id] > 0)
			continue;
		marker++;
		getNext(id, marker, visited);
	}
	while (f = leadingView.getNextFeature()) {
		int id = f->GetFID();
		f->SetField("cluster_id", visited[id]);
	}
}
