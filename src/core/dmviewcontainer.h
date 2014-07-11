#ifndef VIEWCONTAINER_H
#define VIEWCONTAINER_H

#include <dmview.h>

#ifdef SWIG
#define DM_HELPER_DLL_EXPORT
#endif

class DM_HELPER_DLL_EXPORT OGRFeature;
class DM_HELPER_DLL_EXPORT OGRFeatureShadow;

namespace DM {

class DM_HELPER_DLL_EXPORT GDALSystem;

class DM_HELPER_DLL_EXPORT ViewContainer : public View
{
private:
	GDALSystem * _currentSys;

	std::vector<OGRFeature *> newFeatures_write;
	std::vector<OGRFeature *> dirtyFeatures_write;
	std::vector<OGRFeature *> dirtyFeatures_read;

	bool readonly;

	void registerFeature(OGRFeature * f);

public:
	ViewContainer();
	virtual ~ViewContainer();
	ViewContainer(std::string name, int type, ACCESS accesstypeGeometry = READ);

	void setCurrentGDALSystem(DM::GDALSystem * sys);

	OGRFeature* createFeature();
	OGRFeatureShadow* createFeatureShadow();
	void syncAlteredFeatures();
	void syncReadFeatures();

	int getFeatureCount();

	void resetReading();

	OGRFeature *getOGRFeature(long nFID);

	OGRFeature *getFeature(long dynamind_id);

	OGRFeature *getNextFeature();



};
}

#endif // VIEWCONTAINER_H
