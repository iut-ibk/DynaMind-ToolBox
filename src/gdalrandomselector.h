#ifndef GDALRANDOMSELECTOR_H
#define GDALRANDOMSELECTOR_H

#include <dmmodule.h>
#include <dm.h>
#include <ogrsf_frmts.h>

class GDALRandomSelector: public DM::Module
{
	DM_DECLARE_NODE(GDALRandomSelector)

public:
	GDALRandomSelector();
	void run();
	void init();

private:
	DM::ViewContainer vc;
	DM::ViewContainer view_from;

	std::string viewName;
	std::string attribute;

	bool valueFromView;
	int elements;
	bool initFailed;
	std::string units;

	std::string viewNameFrom;
	std::string attributeNameFrom;


};

#endif // GDALRANDOMSELECTOR_H
