#ifndef VIEWCONTAINER_H
#define VIEWCONTAINER_H

#include <dmview.h>

class OGRFeature;

namespace DM {

class GDALSystem;

class ViewContainer : public View
{
private:
	GDALSystem * _currentSys;

	std::vector<OGRFeature *> dirtyFeatures;

public:
	ViewContainer();
	~ViewContainer();
	ViewContainer(std::string name, int type, ACCESS accesstypeGeometry = READ);

	void setCurrentGDALSystem(DM::GDALSystem * sys);

	OGRFeature *createFeature();
	void syncFeatures();

	int getFeatureCount();

	void resetReading();

	OGRFeature *getFeature(long nFID);




};
}

#endif // VIEWCONTAINER_H
