#include "directnetwork.h"


#include <ogr_api.h>
#include <ogrsf_frmts.h>


DM_DECLARE_NODE_NAME(GDALDirectNetwork, GDALModules)


GDALDirectNetwork::GDALDirectNetwork()
{
	GDALModule = true;

	network = DM::ViewContainer("network", DM::EDGE, DM::READ);
	network.addAttribute("cluster_id",  DM::Attribute::INT, DM::READ);
	network.addAttribute("start_id",  DM::Attribute::INT, DM::READ);
	network.addAttribute("end_id",  DM::Attribute::INT, DM::READ);

	junctions = DM::ViewContainer("node", DM::NODE, DM::READ);
	junctions.addAttribute("node_id",  DM::Attribute::INT, DM::READ);
	junctions.addAttribute("height",  DM::Attribute::DOUBLE, DM::READ);
	junctions.addAttribute("height_diff",  DM::Attribute::DOUBLE, DM::WRITE);
	junctions.addAttribute("possible_endpoint", DM::Attribute::INT, DM::WRITE);

	std::vector<DM::ViewContainer*> data_stream;
	data_stream.push_back(&network);
	data_stream.push_back(&junctions);

	this->registerViewContainers(data_stream);
}


double GDALDirectNetwork::walk_the_edge(std::map<long, std::pair<long, long> > & edge_list, std::map<long, std::vector<long> > & start_nodes, std::map<long, int> & edge_cluster,  int start_id, double current_height)
{
	//Get Connecting Edge
	std::vector<long> edges = start_nodes[start_id];
	double height_start = this->node_depth[start_id];
	if (height_start == 0) {
		height_start = current_height;
	}
	double height_end = 0;
	double height_diff = 0;

	foreach(long e_id, edges) {
		if (edge_cluster[e_id] != -1)
			continue;
		edge_cluster[e_id] = 1;
		std::pair<long, long> edge = edge_list[e_id];
		long s_id = edge.first;
		long end_id = edge.second;
		long next_id = -1;
		if (end_id != start_id) {
			next_id = end_id;
			height_end = this->node_depth[end_id];
		}
		else{
			next_id = s_id;
			height_end = this->node_depth[s_id];
		}
		//Caluclate delta h;
		if (height_start != 0.0 && height_end != 0.0)
			height_diff += height_start - height_end;
		height_diff += walk_the_edge(edge_list, start_nodes, edge_cluster, next_id, height_start);
	}

	return height_diff;
}

void GDALDirectNetwork::run()
{
	network.resetReading();

	OGRFeature * f;
	junctions.resetReading();
	while (f = junctions.getNextFeature()) {
		this->node_depth[f->GetFieldAsInteger("node_id")] = f->GetFieldAsDouble("height");
		this->n_id_to_fid[f->GetFieldAsInteger("node_id")] = f->GetFID();
	}

	//edge_id, cluster id
	std::map<long, std::pair<long, long> > edge_list;
	std::map<long, std::vector<long> > start_nodes;
	std::map<long, int> edge_cluster;
	std::map<long, double> node_id_delta;
	std::map<long, std::vector<long> > end_point_list;
	network.resetReading();
	//network.setAttributeFilter("cluster_id = 689");
	//Init Node List
	while (f = network.getNextFeature()) {
		long start_id =f->GetFieldAsInteger("start_id");
		long end_id =f->GetFieldAsInteger("end_id");
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

	//Find possble endpoints
	for(int i = 0; i < start.size(); i++) {
		for (std::map<long, int>::const_iterator it = edge_cluster.begin(); it != edge_cluster.end(); ++it) {
			edge_cluster[it->first] = -1;
		}
		int start_id = start[i];
		double sum_delta = walk_the_edge(edge_list, start_nodes, edge_cluster, start_id);
		node_id_delta[start[i]] = sum_delta;

		OGRFeature * e  = this->network.getFeature(start_nodes[start_id][0]);
		long cluster_id = e->GetFieldAsInteger("cluster_id");
		if (end_point_list.count(cluster_id) == 0) {
			end_point_list[cluster_id] = std::vector<long>();
			std::vector<long> & el =  end_point_list[cluster_id];
			el.push_back(start_id);
		} else {
			std::vector<long> & el =  end_point_list[cluster_id];
			double current_el = node_id_delta[el[0]];
			if (current_el > sum_delta) {
				end_point_list[cluster_id] = std::vector<long>();
				std::vector<long> & el =  end_point_list[cluster_id];
				el.push_back(start_id);
			} else if (current_el == sum_delta) {
				std::vector<long> & el =  end_point_list[cluster_id];
				el.push_back(start_id);
			}
		}
	}
	std::map<long, long> possible_endpoints_sorted;

	for (std::map<long, std::vector<long> >::const_iterator it = end_point_list.begin(); it != end_point_list.end(); ++it) {
		std::vector<long> el = it->second;
		foreach (long e, el) {
			possible_endpoints_sorted[e] = it->first;
		}
	}

	for (std::map<long, double>::const_iterator it = node_id_delta.begin(); it != node_id_delta.end(); ++it) {
		OGRFeature * f = junctions.getFeature(n_id_to_fid[it->first]);
		if (possible_endpoints_sorted.count(it->first) > 0) {
			f->SetField("possible_endpoint", (int) possible_endpoints_sorted[it->first]);
		}

		f->SetField("height_diff", it->second);
	}

}


