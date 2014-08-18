#ifndef GDALREMOVECOMPONETS_H
#define GDALREMOVECOMPONETS_H

#include <dmmodule.h>
#include <dm.h>
#include <ogrsf_frmts.h>

class GDALRemoveComponets : public DM::Module
{
	DM_DECLARE_NODE(GDALRemoveComponets)
public:
	GDALRemoveComponets();
	void init();
	void run();

private:
	DM::ViewContainer vc;
	std::string viewName;
};

#endif // GDALREMOVECOMPONETS_H
