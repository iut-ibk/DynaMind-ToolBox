#ifndef GDALSPATIALLINKING_H
#define GDALSPATIALLINKING_H

#include <dmmodule.h>
#include <dm.h>

class DM_HELPER_DLL_EXPORT GDALSpatialLinking : public DM::Module
{
	DM_DECLARE_NODE(GDALSpatialLinking)
public:
	GDALSpatialLinking();
	void init();
	void run();
private:
	bool properInit; // is true when init succeeded

	std::string leadingViewName;
	std::string linkViewName;

	DM::ViewContainer leadingView;
	DM::ViewContainer linkView;

	std::string link_name;

};

#endif // GDALSPATIALLINKING_H
