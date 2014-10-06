#ifndef GDALCLUSTERNEIGHBOURHOOD_H
#define GDALCLUSTERNEIGHBOURHOOD_H

#include <dmmodule.h>

class GDALClusterNeighbourhood: public DM::Module
{
	DM_DECLARE_NODE(GDALClusterNeighbourhood)
public:
	GDALClusterNeighbourhood();
	void init();
	void run();

private:
	std::string viewName;
	std::string neighName;

	std::string name_id1;
	std::string name_id2;

	DM::ViewContainer leadingView;
	DM::ViewContainer ngView;

	void getNext(int id, int marker, std::map<int, int> & visited);
};

#endif // GDALCLUSTERNEIGHBOURHOOD_H
