#include "dm_gaphattributes.h"
#include <dm.h>
#include <ogr_api.h>
#include <ogrsf_frmts.h>

DM_DECLARE_NODE_NAME(DM_GaphAttributes, Network Analysis)



DM_GaphAttributes::DM_GaphAttributes()
{
	GDALModule = true;

	this->view_name = "network";
	this->addParameter("view_name", DM::STRING, &view_name);

	this->attribute_read = "attribute_read";
	this->addParameter("attribute_read", DM::STRING, &attribute_read);

	this->attribute_write = "attribute_write";
	this->addParameter("attribute_write", DM::STRING, &attribute_write);

}

void DM_GaphAttributes::init()
{
	network = DM::ViewContainer(view_name, DM::EDGE, DM::READ);
	network.addAttribute(attribute_read,  DM::Attribute::DOUBLE, DM::READ);
	network.addAttribute(attribute_write,  DM::Attribute::DOUBLE, DM::WRITE);

	std::vector<DM::ViewContainer*> data_stream;
	data_stream.push_back(&network);

	this->registerViewContainers(data_stream);
}

void DM_GaphAttributes::run()
{
	network.resetReading();
	// [end_id] -----> [start_id]
	OGRFeature * f;
	DM::Logger(DM::Standard) << "Build search structure";
	int counter = 0;
	while (f = network.getNextFeature()) {
		counter++;
		if (counter % 10000 == 0) {
			DM::Logger(DM::Debug) << counter;
		}
		long start_id = f->GetFieldAsInteger("start_id"); // downstream
		long end_id = f->GetFieldAsInteger("end_id"); //upstream

		if (start_id == end_id)
			continue;

		construction_age[f->GetFID()] = f->GetFieldAsInteger(this->attribute_read.c_str());
		visitor_id[f->GetFID()] = -1;
		edge_list[f->GetFID()] = std::pair<long, long>(start_id, end_id);

		if (start_nodes.count(end_id) == 0)
			start_nodes[end_id] = std::vector<long>();

		if (end_nodes.count(start_id) == 0)
			end_nodes[start_id] = std::vector<long>();

		if (node_con_counter.find(start_id) == node_con_counter.end())
			node_con_counter[start_id] = 0;

		if (node_con_counter.find(end_id) == node_con_counter.end())
			node_con_counter[end_id] = 0;

		node_con_counter[start_id] = node_con_counter[start_id] + 1;
		node_con_counter[end_id] = node_con_counter[end_id] + 1;

		std::vector<long> & vec_end = start_nodes[end_id];
		vec_end.push_back(f->GetFID());

		std::vector<long> & vec_start = end_nodes[start_id];
		vec_start.push_back(f->GetFID());
	}

	DM::Logger(DM::Debug) << "Start search form leafs";
	int start_nodes = 0;
	for (std::map<long, long >::const_iterator it = node_con_counter.begin();
		 it != node_con_counter.end();
		 it++) {
		if (it->second != 1)
			continue;
		start_nodes++;
	}
	counter = 0;
	for (std::map<long, long >::const_iterator it = node_con_counter.begin();
		 it != node_con_counter.end();
		 it++) {
		if (it->second != 1)
			continue;
		counter++;
		if (counter % 1000 == 0) {
			DM::Logger(DM::Debug) << counter << "/" << start_nodes;
		}
		// Get starting edge with inital strahler number 1
		std::set<long> visted;
		getNext(it->first, 9999, visted);
	}
	DM::Logger(DM::Debug) << counter << "/" << start_nodes;
	// Write Strahler
	DM::Logger(DM::Debug) << "Write number";
	network.resetReading();
	// [end_id] -----> [start_id]
	while (f = network.getNextFeature()) {
		f->SetField(this->attribute_write.c_str(), construction_age[f->GetFID()]);
	}
}

void DM_GaphAttributes::getNext(long node_id, long current_min_age, std::set<long> &visted)
{
	// [end_id] -----> [start_id]
	if (start_nodes.count(node_id) == 0) {
		return;
	}

	std::vector<long> start_edges = start_nodes[node_id];
	bool change_next = false;

	// Start Node
	std::vector<long> edges_at_start = end_nodes[node_id];


	foreach (long edge, start_edges) {
		// Get next edge
		std::pair<long, long> e = edge_list[edge];
		long next = e.first;

		if (construction_age[edge] < current_min_age)
			current_min_age = construction_age[edge];

		construction_age[edge] = current_min_age;

		if (visted.find(edge) != visted.end()) {
			DM::Logger(DM::Error) << "Break Loop";
			continue;
		}
		visted.insert(edge);
		getNext(next, current_min_age, visted);
		break; //Only follow one path
	}
}
