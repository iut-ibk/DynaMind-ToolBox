#ifndef VIEWCONTAINER_H
#define VIEWCONTAINER_H

#include <dmview.h>

class OGRFeature;
class OGRFeatureShadow;

namespace DM {

class GDALSystem;

class ViewContainer : public View
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
