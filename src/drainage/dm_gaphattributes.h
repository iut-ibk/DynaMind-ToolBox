#ifndef DM_GAPHATTRIBUTES_H
#define DM_GAPHATTRIBUTES_H

#include <dmmodule.h>
class DM_HELPER_DLL_EXPORT DM_GaphAttributes : public DM::Module
{
	DM_DECLARE_NODE (DM_GaphAttributes)

private:
	std::string view_name;

	std::string attribute_read;
	std::string attribute_write;

	DM::ViewContainer network;

	std::map<long, std::pair<long, long> > edge_list; // edge_id <start_id, end_id>
	std::map<long, int> construction_age;
	std::map<long, int> visitor_id;
	std::map<long, std::vector<long> > start_nodes; // start_id <edge_id>
	std::map<long, std::vector<long> > end_nodes; // end_id <edge_id>
	std::map<long,long> node_con_counter;
	void getNext(long node_id, long current_min_age, std::set<long> &visted);


public:
	DM_GaphAttributes();
	void init();
	void run();
};

#endif // DM_GAPHATTRIBUTES_H
