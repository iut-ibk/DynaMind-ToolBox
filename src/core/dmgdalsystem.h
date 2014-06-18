#ifndef DMGDALSYSTEM_H
#define DMGDALSYSTEM_H

#include <ogrsf_frmts.h>
#include <dmview.h>

#include <map>
#include <vector>

class OGRDataSource;
class OGRSFDriver;
class OGRLayer;

namespace DM {
class GDALSystem
{
public:
	GDALSystem();

	GDALSystem(const GDALSystem& s);

	void syncFeatures(const DM::View & v);

	void updateSystemView(const DM::View & v);

	OGRFeature *createFeature(const DM::View & v);

	OGRLayer *getOGRLayer(const DM::View & v);

	void resetReading(const DM::View & v);

	GDALSystem *createSuccessor();








private:
	OGRDataSource		*poDS;
	OGRSFDriver			*poDrive;
	std::map<std::string, OGRLayer *> viewLayer;
	std::vector<std::string> state_ids;

	std::vector<OGRFeature *> dirtyFeatures;

};
}

#endif // DMGDALSYSTEM_H
