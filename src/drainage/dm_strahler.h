#ifndef DM_STRAHLER_H
#define DM_STRAHLER_H

#include <dmmodule.h>

class DM_HELPER_DLL_EXPORT DM_Strahler : public DM::Module
{
	DM_DECLARE_NODE (DM_Strahler)

private:
	std::string view_name;
	DM::ViewContainer network;


	std::map<long, std::pair<long, long> > edge_list; // edge_id <start_id, end_id>
	std::map<long, int> strahler_id;
	std::map<long, int> visitor_id;
	std::map<long, std::vector<long> > start_nodes; // start_id <edge_id>
	std::map<long, std::vector<long> > end_nodes; // end_id <edge_id>
	std::map<long,long> node_con_counter;
	void getNext(long node_id, long current_strahler, std::set<long> &visted);

public:
	DM_Strahler();
	void init();
	void run();
};

#endif // DM_STRAHLER_H
