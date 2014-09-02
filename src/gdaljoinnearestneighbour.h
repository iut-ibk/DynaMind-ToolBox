#ifndef GDALJOINNEARESTNEIGHBOUR_H
#define GDALJOINNEARESTNEIGHBOUR_H

#include <dmmodule.h>
#include <dm.h>

class DM_HELPER_DLL_EXPORT GDALJoinNearestNeighbour : public DM::Module
{
		DM_DECLARE_NODE(GDALJoinNearestNeighbour)
public:

	GDALJoinNearestNeighbour();

	void init();
	void run();
private:
	bool properInit; // is true when init succeeded

	std::string leadingViewName;
	std::string linkViewName;

	DM::ViewContainer leadingView;
	DM::ViewContainer linkView;

	double max_distance;

	std::string link_name;
};

#endif // GDALJOINNEARESTNEIGHBOUR_H
