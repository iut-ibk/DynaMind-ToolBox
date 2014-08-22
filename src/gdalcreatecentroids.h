#ifndef GDALCREATECENTROIDS_H
#define GDALCREATECENTROIDS_H

#include <dmmodule.h>
#include <dm.h>

class DM_HELPER_DLL_EXPORT GDALCreateCentroids : public DM::Module
{
	DM_DECLARE_NODE(GDALCreateCentroids)
public:
	GDALCreateCentroids();

	void run();
	void init();

private:
	std::string link_name;

	std::string leadingViewName;
	std::string centroidViewName;

	DM::ViewContainer leadingView;
	DM::ViewContainer centroidView;


};

#endif // GDALCREATECENTROIDS_H
