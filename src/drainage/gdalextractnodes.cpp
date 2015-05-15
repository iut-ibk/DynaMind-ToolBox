#include "gdalextractnodes.h"


#include <ogr_api.h>
#include <ogrsf_frmts.h>


DM_DECLARE_NODE_NAME(GDALExtractNodes, GDALModules)


GDALExtractNodes::GDALExtractNodes()
{
	GDALModule = true;

	network = DM::ViewContainer("network", DM::EDGE, DM::READ);
	network.addAttribute("start_id",  DM::Attribute::INT, DM::WRITE);
	network.addAttribute("end_id",  DM::Attribute::INT, DM::WRITE);
	network.addAttribute("level_lo", DM::Attribute::DOUBLE, DM::READ);
	network.addAttribute("level_up", DM::Attribute::DOUBLE, DM::READ);

	junctions = DM::ViewContainer("node", DM::NODE, DM::WRITE);
	junctions.addAttribute("node_id",  DM::Attribute::INT, DM::WRITE);
	junctions.addAttribute("height",  DM::Attribute::DOUBLE, DM::WRITE);

	std::vector<DM::ViewContainer*> data_stream;
	data_stream.push_back(&network);
	data_stream.push_back(&junctions);

	tolerance = 0.01;
	this->addParameter("tolerance", DM::DOUBLE, &tolerance);

	is_downstream_upstream = true;
	this->addParameter("is_downstream_upstream", DM::BOOL, &is_downstream_upstream);

	registerViewContainers(data_stream);
	this->addParameter("tolerance", DM::DOUBLE, &tolerance);
}

long GDALExtractNodes::getNodeID( OGRPoint &  p1, std::map<std::pair<long, long>, std::pair<long, OGRFeature *> > & node_list, double elev)
{
	double x = p1.getX();
	double y = p1.getY();
	std::pair<long, long> node_hash ((long) (x/tolerance), (long) (y/tolerance));
	if (node_list.count(node_hash) == 0) {
		node_id++;

		OGRFeature * j = junctions.createFeature();
		j->SetField("node_id", (int)node_id);
		j->SetField("height", elev);
		j->SetGeometry(&p1);
		node_list[node_hash] = std::pair<long, OGRFeature *>(node_id,j);
		return node_id;
	} else {
		std::pair<long, OGRFeature *> node =  node_list[node_hash];
		//Check if height is lower than the current height
		OGRFeature * f = node.second;
		if (elev != 0.0 && elev < f->GetFieldAsDouble("height")) {
			f->SetField("height", elev);
		}
		return node.first;
	}
}


void GDALExtractNodes::run()
{
	network.resetReading();

	OGRFeature * f;

	//contains a coordinate pair (x,y) and an id

	std::map<std::pair<long, long>, std::pair<long, OGRFeature *> > node_list;

	//edge_id, cluster id
	std::map<long, int> edge_cluster;
	std::map<long, std::pair<long, long> > edge_list;

	node_id = 0;

	//Init Node List
	while (f = network.getNextFeature()) {
		OGRLineString * edge = (OGRLineString *)f->GetGeometryRef();
		if (!edge)
			continue;

		int points = edge->getNumPoints();
		OGRPoint p1;
		OGRPoint p2;
		if (is_downstream_upstream) {
			edge->getPoint(0, &p1);
			edge->getPoint(points-1, &p2);
		} else {
			edge->getPoint(0, &p2);
			edge->getPoint(points-1, &p1);
		}
		double down = f->GetFieldAsDouble("level_lo");
		double up = f->GetFieldAsDouble("level_up");

		long start_id = getNodeID( p1, node_list, down);
		long end_id = getNodeID( p2, node_list, up);

		if (start_id == end_id)
			continue;

		edge_cluster[f->GetFID()] = -1;
		edge_list[f->GetFID()] = std::pair<long, long>(start_id, end_id);

	}

	network.resetReading();
	while (f = network.getNextFeature()) {
		f->SetField("start_id", (int) edge_list[f->GetFID()].first);
		f->SetField("end_id", (int) edge_list[f->GetFID()].second);
	}
}


