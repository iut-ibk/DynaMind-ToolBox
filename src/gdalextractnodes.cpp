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
	network.addAttribute("upstream_i", DM::Attribute::DOUBLE, DM::READ);
	network.addAttribute("downstre_1", DM::Attribute::DOUBLE, DM::READ);

	junctions = DM::ViewContainer("node", DM::NODE, DM::WRITE);
	junctions.addAttribute("node_id",  DM::Attribute::INT, DM::WRITE);
	junctions.addAttribute("height",  DM::Attribute::DOUBLE, DM::WRITE);

	std::vector<DM::ViewContainer*> data_stream;
	data_stream.push_back(&network);
	data_stream.push_back(&junctions);

	this->registerViewContainers(data_stream);
}

int GDALExtractNodes::getNodeID(long & node_id, OGRPoint &  p1, std::map<std::pair<long, long>, long > & node_list, double tolerance, double elev)
{
	double x = p1.getX();
	double y = p1.getY();
	std::pair<long, long> node_hash ((long)x*tolerance, (long)y*tolerance);
	if (node_list.count(node_hash) == 0) {
		node_id++;
		node_list[node_hash] = node_id;
		OGRFeature * j = junctions.createFeature();
		j->SetField("node_id", (int)node_id);
		j->SetField("height", elev);
		j->SetGeometry(&p1);
		return node_id;
	} else {
		return node_list[node_hash];
	}
}


void GDALExtractNodes::run()
{
	double tolerance = 100;
	network.resetReading();

	OGRFeature * f;

	//contains a coordinate pair (x,y) and an id
	std::map<std::pair<long, long>, long > node_list;

	//edge_id, cluster id
	std::map<long, int> edge_cluster;
	std::map<long, std::pair<long, long> > edge_list;
	std::map<long, std::vector<long> > start_nodes;

	long node_id = 0;

	//Init Node List
	while (f = network.getNextFeature()) {
		OGRLineString * edge = (OGRLineString *)f->GetGeometryRef();
		if (!edge)
			continue;

		int points = edge->getNumPoints();
		OGRPoint p1;
		OGRPoint p2;
		edge->getPoint(0, &p1);
		edge->getPoint(points-1, &p2);
		double up = f->GetFieldAsDouble("upstream_i");
		double down = f->GetFieldAsDouble("downstre_1");

		long start_id = getNodeID(node_id, p1, node_list, tolerance, up);
		long end_id = getNodeID(node_id, p2, node_list, tolerance, down);

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


