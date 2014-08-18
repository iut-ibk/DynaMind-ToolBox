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

	std::string viewName;
	std::string attribute;
	int elements;
	bool initFailed;


};

#endif // GDALRANDOMSELECTOR_H
