#ifndef GDALMERGEFACES_H
#define GDALMERGEFACES_H

#include <dmmodule.h>


class GDALMergeFaces : public DM::Module
{
	DM_DECLARE_NODE(GDALMergeFaces)
public:
	GDALMergeFaces();
	void init();
	void run();

private:
	std::string viewName;
	std::string combinedViewName;

	std::string attriubteName;

	DM::ViewContainer leadingView;
	DM::ViewContainer combinedView;
	OGRGeometry * joinCluster(int cluster);
};

#endif // GDALMERGEFACES_H
