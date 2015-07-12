#ifndef GDALCLUSTERNETWORK_H
#define GDALCLUSTERNETWORK_H

#include <dmmodule.h>
#include <dm.h>

class DM_HELPER_DLL_EXPORT GDALClusterNetwork : public DM::Module
{
	DM_DECLARE_NODE(GDALClusterNetwork);
private:

	DM::ViewContainer network;
	DM::ViewContainer junctions;

	std::string view_name;

	//edge_id, cluster id
	std::map<long, int> edge_cluster;
	std::map<long, std::pair<long, long> > edge_list;
	std::map<long, std::vector<long> > start_nodes;

public:
	GDALClusterNetwork();
	void run();
	void init();

	int getNodeID(long &node_id, OGRPoint &p1, std::map<std::pair<long, long>, long> &node_list, double tolerance, double elev = 0);
	std::vector<long> walk_the_edge(std::vector<long> start_id, int cluster_id);
};

#endif // GDALCLUSTERNETWORK_H
