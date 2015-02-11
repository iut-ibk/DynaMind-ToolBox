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

int GDALClusterNetwork::getNodeID(long & node_id, OGRPoint &  p1, std::map<std::pair<long, long>, long > & node_list, double tolerance)
{
	double x = p1.getX();
	double y = p1.getY();
	std::pair<long, long> node_hash ((long)x*tolerance, (long)y*tolerance);
	if (node_list.count(node_hash) == 0) {
		node_id++;
		node_list[node_hash] = node_id;
		return node_id;
	} else {
		return node_list[node_hash];
	}
}

void GDALClusterNetwork::walk_the_edge(std::map<long, std::pair<long, long> > & edge_list, std::map<long, std::vector<long> > & start_nodes, std::map<long, int> & edge_cluster, int & marker, int start_id, bool top)
{
	std::vector<long> edges = start_nodes[start_id];
	foreach(long e_id, edges) {
		if (edge_cluster[e_id] != -1)
			continue;
		//Get Edge
		if (top)
			marker++;
		top = false;
		edge_cluster[e_id] = marker;
		std::pair<long, long> edge = edge_list[e_id];
		long s_id = edge.first;
		long end_id = edge.second;
		long next_id = -1;
		if (end_id != start_id)
			next_id = end_id;
		else
			next_id = s_id;
		walk_the_edge(edge_list, start_nodes,  edge_cluster,  marker, next_id);
	}
}

void GDALClusterNetwork::run()
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
	//std::map<long,  std::vector<long> > end_nodes;

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

		long start_id = getNodeID(node_id, p1, node_list, tolerance);
		long end_id = getNodeID(node_id, p2, node_list, tolerance);

		edge_cluster[f->GetFID()] = -1;
		edge_list[f->GetFID()] = std::pair<long, long>(start_id, end_id);

		if (start_nodes.count(start_id) == 0)
			start_nodes[start_id] = std::vector<long>();

		if (start_nodes.count(end_id) == 0)
			start_nodes[end_id] = std::vector<long>();

		std::vector<long> & vec_start =  start_nodes[start_id];
		vec_start.push_back(f->GetFID());
		std::vector<long> & vec_end = start_nodes[end_id];
		vec_end.push_back(f->GetFID());
	}

	//Start point list
	std::vector<long> start;
	for (std::map<long,  std::vector<long> >::const_iterator it = start_nodes.begin(); it != start_nodes.end(); ++it) {
		//DM::Logger(DM::Error) << "start node " << it->first << "/" << it->second.size();
		if(it->second.size() == 1) {
			start.push_back(it->first);
			//DM::Logger(DM::Error) << "start node " << it->first;
		}
	}
	int marker = 0;
	for(int i = 0; i < start.size(); i++) {
		int start_id = start[i];
		//marker++;
		walk_the_edge(edge_list, start_nodes, edge_cluster, marker, start_id, true);
	}

	network.resetReading();
	while (f = network.getNextFeature()) {
		f->SetField("cluster_id", edge_cluster[f->GetFID()]);
	}




}


