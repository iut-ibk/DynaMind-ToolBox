#ifndef DMGDALSYSTEM_H
#define DMGDALSYSTEM_H

#include <ogrsf_frmts.h>
#include <dmview.h>

#include <map>

class OGRDataSource;
class OGRSFDriver;
class OGRLayer;

namespace DM {
class GDALSystem
{
public:
	GDALSystem();

	void syncFeatures(const DM::View & v);

	void updateSystemView(const DM::View & v);

	OGRFeature * createFeature(const DM::View & v);

	std::map<std::string, OGRLayer *> viewLayer;



private:
	OGRDataSource		*poDS;
	OGRSFDriver			*poDrive;



	std::vector<OGRFeature *> dirtyFeatures;

};
}

#endif // DMGDALSYSTEM_H
