#include "gdalclusternetwork.h"

#include <ogr_api.h>
#include <ogrsf_frmts.h>

DM_DECLARE_CUSTOM_NODE_NAME(GDALClusterNetwork,Identify Clusters of Connected Lines, Network Generation)

GDALClusterNetwork::GDALClusterNetwork()
{
	GDALModule = true;

	this->view_name = "network";
	this->addParameter("view_name", DM::STRING, &view_name);
}

void GDALClusterNetwork::init()
{
	network = DM::ViewContainer(view_name, DM::EDGE, DM::READ);
	network.addAttribute("cluster_id",  DM::Attribute::INT, DM::WRITE);
	network.addAttribute("start_id",  DM::Attribute::INT, DM::READ);
	network.addAttribute("end_id",  DM::Attribute::INT, DM::READ);

	std::vector<DM::ViewContainer*> data_stream;
	data_stream.push_back(&network);

	this->registerViewContainers(data_stream);
}

string GDALClusterNetwork::getHelpUrl()
{
	return "/DynaMind-GDALModules/gdalclusternetwork.html";
}

std::vector<long> GDALClusterNetwork::walk_the_edge(std::vector<long> start_ids, int cluster_id)
{
	std::vector<long> next_nodes;

	foreach(long start_id, start_ids) {
		if (start_nodes.find(start_id) == start_nodes.end()) {
			DM::Logger(DM::Error) << "No start node at " << start_id << " found ";
			continue;
		}
		std::vector<long> & edges = start_nodes[start_id];
		foreach(long e_id, edges) {
			if (edge_cluster[e_id] != -1)
				continue;
			edge_cluster[e_id] = cluster_id;
			std::pair<long, long> edge = edge_list[e_id];
			long s_id = edge.first;
			long end_id = edge.second;
			long next_id = -1;
			if (end_id != start_id)
				next_id = end_id;
			else
				next_id = s_id;
			next_nodes.push_back(next_id);
		}
	}
	return next_nodes;
}

void GDALClusterNetwork::run()
{
	network.resetReading();

	OGRFeature * f;

	edge_cluster.clear();
	edge_list.clear();
	start_nodes.clear();

	//Init Node List
	while (f = network.getNextFeature()) {
		long start_id = f->GetFieldAsInteger("start_id");
		long end_id = f->GetFieldAsInteger("end_id");

		if (start_id == end_id)
			continue;

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
		if(it->second.size() == 1) {
			start.push_back(it->first);
		}
	}

	int cluster_id = 1;
	int next_cluster_id = 1;
	for(int i = 0; i < start.size(); i++) {
		int start_id = start[i];
		std::vector<long> start_ids;
		start_ids.push_back(start_id);
		int recursion_depth = 0;

		while (start_ids.size() > 0) {
			start_ids = walk_the_edge(start_ids, cluster_id);
			if (recursion_depth == 1) { //Create new cluster id for next run
				next_cluster_id = cluster_id+1;
			}
			recursion_depth++;
		}
		cluster_id = next_cluster_id;
	}
	network.resetReading();
	while (f = network.getNextFeature()) {
		f->SetField("cluster_id", edge_cluster[f->GetFID()]);
	}
}




