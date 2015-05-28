#ifndef GDALEXTRACTNODES_H
#define GDALEXTRACTNODES_H

#include <dmmodule.h>
#include <dm.h>

class DM_HELPER_DLL_EXPORT GDALExtractNodes : public DM::Module
{
	DM_DECLARE_NODE(GDALExtractNodes);
private:

	DM::ViewContainer network;
	DM::ViewContainer junctions;

	std::string view_name;
	std::string node_name;
	double tolerance;
	long node_id;
	bool is_downstream_upstream;
	bool add_height;


public:
	GDALExtractNodes();
	void run();
	void init();
	long getNodeID(OGRPoint &p1, std::map<std::pair<long, long>, std::pair<long, OGRFeature *> > &node_list, double elev = 0);

};

#endif // GDALEXTRACTNODES_H
