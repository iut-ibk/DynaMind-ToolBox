#ifndef DM_SIMPLIFYNETWORK_H
#define DM_SIMPLIFYNETWORK_H

#include <ogr_api.h>
#include <ogrsf_frmts.h>
#include <dmmodule.h>

class DM_HELPER_DLL_EXPORT DM_SimplifyNetwork : public DM::Module
{
	DM_DECLARE_NODE (DM_SimplifyNetwork)


private:
	std::string view_name;
	std::string out_name;

	int root_node;

	DM::ViewContainer network;
	DM::ViewContainer out_network;
	DM::ViewContainer out_node;


	std::map<long, std::pair<long, long> > edge_list; // edge_id <start_id, end_id>
	std::map<long, int> strahler_id;
	// std::map<long, int> visitor_id;
	// std::map<long, std::vector<long> > start_nodes; // start_id <edge_id>
	std::map<long, std::vector<long> > start_nodes; // end_id <edge_id>
	// std::map<long,long> node_con_counter;
	std::map<long, OGRPoint> node_point_map;

	std::map<long, long> visited;

public:
	DM_SimplifyNetwork();
	void init();
	void run();

	void getNext(long edge_id, long start_node);
	void createEdge(long node_id, long start_node, long strahler);
};

#endif // DM_SIMPLIFYNETWORK_H
