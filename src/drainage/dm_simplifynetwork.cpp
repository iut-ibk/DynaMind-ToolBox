#include "dm_simplifynetwork.h"

#include <dm.h>
#include <ogr_api.h>
#include <ogrsf_frmts.h>


DM_DECLARE_NODE_NAME(DM_SimplifyNetwork, NetworkAnalysis)

DM_SimplifyNetwork::DM_SimplifyNetwork()
{
	GDALModule = true;

	this->view_name = "network";
	this->addParameter("view_name", DM::STRING, &view_name);

	this->out_name = "network_out";
	this->addParameter("out_name", DM::STRING, &out_name);

	this->root_node = 0;
	this->addParameter("root_node", DM::INT, &this->root_node);

}

void DM_SimplifyNetwork::init()
{
	network = DM::ViewContainer(view_name, DM::EDGE, DM::READ);
	network.addAttribute("strahler_number",  DM::Attribute::INT, DM::READ);
	network.addAttribute("start_id",  DM::Attribute::INT, DM::READ);
	network.addAttribute("end_id",  DM::Attribute::INT, DM::READ);
	out_network = DM::ViewContainer(out_name, DM::EDGE, DM::WRITE);
	out_network.addAttribute("start_id",  DM::Attribute::INT, DM::WRITE);
	out_network.addAttribute("end_id",  DM::Attribute::INT, DM::WRITE);
	out_network.addAttribute("strahler_number",  DM::Attribute::INT, DM::WRITE);

	std::vector<DM::ViewContainer*> data_stream;
	data_stream.push_back(&network);
	data_stream.push_back(&out_network);

	this->registerViewContainers(data_stream);
}

void DM_SimplifyNetwork::createEdge(long node_id, long start_node, long strahler)
{
	OGRLineString ls;
	OGRPoint start_point = node_point_map[start_node];
	OGRPoint end_point = node_point_map[node_id];
	ls.addPoint(&start_point);
	ls.addPoint(&end_point);

	OGRFeature * feature = out_network.createFeature();
	feature->SetField("strahler_number", (int)strahler);
	feature->SetGeometry(&ls);
}

void DM_SimplifyNetwork::getNext(long edge_id, long start_node)
{
	std::pair<long, long> edge = this->edge_list[edge_id];
	long node_id = edge.second;
	if (start_nodes.find(node_id) == start_nodes.end() ||
			visited[edge_id] == 1) {
		createEdge( start_node, node_id, 1);
		return;
	}
	visited[edge_id] = 1;
	std::vector<long> next_edges = start_nodes[node_id];
	// Create Edge
	if (next_edges.size() > 1) {
		createEdge(start_node, node_id, strahler_id[edge_id]);
		start_node = node_id;
	}

	foreach (long e, next_edges) {
		//long start_id = edge_list[e].second;
		getNext(e, start_node);
	}
}

void DM_SimplifyNetwork::run()
{
	network.resetReading();
	// [start_id] -----> [end_id]
	OGRFeature * f;
	while (f = network.getNextFeature()) {
		long start_id = f->GetFieldAsInteger("start_id"); // downstream
		long end_id = f->GetFieldAsInteger("end_id"); //upstream

		if (start_id == end_id)
			continue;

		if (start_nodes.count(start_id) == 0)
			start_nodes[start_id] = std::vector<long>();

		std::vector<long> & vec_start = start_nodes[start_id];
		vec_start.push_back(f->GetFID());

		OGRLineString * ls = (OGRLineString *) f->GetGeometryRef();

		OGRPoint start_point;
		OGRPoint end_point;

		ls->StartPoint(&end_point);
		ls->EndPoint(&start_point);

		node_point_map[start_id] = start_point;
		node_point_map[end_id] = end_point;

		edge_list[f->GetFID()] = std::pair<long, long>(start_id, end_id);
		strahler_id[f->GetFID()] = f->GetFieldAsInteger("strahler_number");
		visited[f->GetFID()] = -1;
	}

	// Get Next Edges
	std::vector<long> next_edges = start_nodes[root_node];
	foreach (long e, next_edges)
		getNext(e, root_node);
}
