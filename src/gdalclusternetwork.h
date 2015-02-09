#ifndef GDALCLUSTERNETWORK_H
#define GDALCLUSTERNETWORK_H

#include <dmmodule.h>
#include <dm.h>

class DM_HELPER_DLL_EXPORT GDALClusterNetwork : public DM::Module
{
	DM_DECLARE_NODE(GDALClusterNetwork);
private:

	DM::ViewContainer network;

public:
	GDALClusterNetwork();
	void run();
};

#endif // GDALCLUSTERNETWORK_H
