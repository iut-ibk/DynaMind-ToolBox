#ifndef GDALLOGATTRIUBTES_H
#define GDALLOGATTRIUBTES_H

#include <dmmodule.h>
#include <dm.h>

class DM_HELPER_DLL_EXPORT GDALLogAttriubtes : public DM::Module
{
	DM_DECLARE_NODE(GDALLogAttriubtes)
public:
	GDALLogAttriubtes();
	void run();
	void init();

private:
	std::string leadingViewName;
	std::vector<std::string> attributeNames;

	DM::ViewContainer leadingView;
};

#endif // GDALLOGATTRIUBTES_H
