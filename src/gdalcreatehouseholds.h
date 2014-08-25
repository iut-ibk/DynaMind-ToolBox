#ifndef GDALCREATEHOUSEHOLDS_H
#define GDALCREATEHOUSEHOLDS_H

#include <dmmodule.h>
#include <dm.h>

class DM_HELPER_DLL_EXPORT GDALCreateHouseholds : public DM::Module
{
	DM_DECLARE_NODE(GDALCreateHouseholds)

public:
	GDALCreateHouseholds();
	void run();
private:
	DM::ViewContainer district;
	DM::ViewContainer building;
	DM::ViewContainer household;

};

#endif // GDALCREATEHOUSEHOLDS_H
