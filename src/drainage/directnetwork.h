#ifndef DIRECTNETWORK_H
#define DIRECTNETWORK_H

#include <dmmodule.h>
#include <dm.h>

class DM_HELPER_DLL_EXPORT GDALDirectNetwork : public DM::Module
{
	DM_DECLARE_NODE(GDALDirectNetwork);
private:

	DM::ViewContainer network;
	DM::ViewContainer junctions;
	std::map<long, double> node_depth;
	std::map<long, long> n_id_to_fid;
public:
	GDALDirectNetwork();
	void run();
	double walk_the_edge(std::map<long, std::pair<long, long> > & edge_list,
						 std::map<long, std::vector<long> > & start_nodes,
						 std::map<long, int> & edge_cluster,
						 int start_id,
						 int current_marker,
						 double current_height = 0);
};


#endif // DIRECTNETWORK_H
