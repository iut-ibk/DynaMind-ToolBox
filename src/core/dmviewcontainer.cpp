#include "dmviewcontainer.h"

#include <dmlogger.h>
#include <dmgdalsystem.h>

namespace DM {
ViewContainer::ViewContainer():
	_currentSys(NULL)
{
}

ViewContainer::~ViewContainer()
{
	//Sync View to DB
	if (_currentSys)
		_currentSys->syncFeatures(*this);
}


ViewContainer::ViewContainer(string name, int type, DM::ACCESS accesstypeGeometry) :
	View(name, type, accesstypeGeometry), _currentSys(NULL)
{

}

void ViewContainer::setCurrentGDALSystem(GDALSystem *sys)
{
	this->_currentSys = sys;
	this->_currentSys->resetReading(*this);

}

OGRFeature *ViewContainer::createFeature()
{
	if (!_currentSys) {
		Logger(Error) << "No GDALSystem registered";
		return NULL;
	}

	return _currentSys->createFeature(*this);

}

void ViewContainer::syncFeatures()
{
	if (!_currentSys) {
		Logger(Error) << "No GDALSystem registered";
		return;
	}

	this->_currentSys->syncFeatures(*this);
}

OGRFeature *ViewContainer::getFeature(long nFID)
{
	if (!_currentSys) {
		Logger(Error) << "No GDALSystem registered";
		return NULL;
	}

	return this->_currentSys->getOGRLayer(*this)->GetFeature(nFID);
}

int ViewContainer::getFeatureCount()
{
	if (!_currentSys) {
		Logger(Error) << "No GDALSystem registered";
		return -1;
	}
	return this->_currentSys->getOGRLayer(*this)->GetFeatureCount();

}

void ViewContainer::resetReading()
{
	if (!_currentSys) {
		Logger(Error) << "No GDALSystem registered";
		return;
	}
	return this->_currentSys->resetReading(*this);
}




}
