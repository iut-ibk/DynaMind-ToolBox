#include "gdalclusternetwork.h"


#include <ogr_api.h>
#include <ogrsf_frmts.h>


DM_DECLARE_NODE_NAME(GDALClusterNetwork, GDALModules)


GDALClusterNetwork::GDALClusterNetwork()
{
	GDALModule = true;

	network = DM::ViewContainer("network", DM::EDGE, DM::READ);
	network.addAttribute("cluster_id", DM::Attribute::INT, DM::WRITE);

	std::vector<DM::ViewContainer*> data_stream;
	data_stream.push_back(&network);

	this->registerViewContainers(data_stream);
}

void GDALClusterNetwork::run()
{
	double tolerance = 100;
	network.resetReading();

	OGRFeature * f;

	std::map<std::pair<long, long>, long > node_list;

	std::map<long, int> edge_cluster;


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

		double x = p1.getX();
		double y = p2.getY();
		int current_node = -1;
		std::pair<long, long> node_hash ((long)x*tolerance, (long)y*tolerance);
		if (node_list.count(node_hash) == 0) {
			node_id++;
			node_list[node_hash] = node_id;
			current_node = node_id;
		} else {
			current_node = node_list[node_hash];
		}
		edge_cluster[f->GetFID()] = -1;
	}


}


