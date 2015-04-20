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

public:
	GDALExtractNodes();
	void run();
	int getNodeID(long &node_id, OGRPoint &p1, std::map<std::pair<long, long>, long> &node_list, double tolerance, double elev = 0);

};

#endif // GDALEXTRACTNODES_H
