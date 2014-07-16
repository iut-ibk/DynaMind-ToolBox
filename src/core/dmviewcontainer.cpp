#include "dmviewcontainer.h"

#include <dmlogger.h>
#include <dmgdalsystem.h>
#include <ogrsf_frmts.h>

namespace DM {

ViewContainer::ViewContainer():
	_currentSys(NULL)
{
}

ViewContainer::~ViewContainer()
{
	//Sync View to DB
	this->syncAlteredFeatures();
	this->syncReadFeatures();
}


ViewContainer::ViewContainer(string name, int type, DM::ACCESS accesstypeGeometry) :
	View(name, type, accesstypeGeometry), _currentSys(NULL)
{

}

void ViewContainer::setCurrentGDALSystem(GDALSystem *sys)
{
	if (!sys) {
		Logger(Error) << "System is not valid";
		return;
	}
	this->_currentSys = sys;
	this->_currentSys->resetReading(*this);
	if (this->writes())
		readonly = false;
	else
		readonly = true;
}

OGRFeature *ViewContainer::createFeature()
{
	if (!_currentSys) {
		Logger(Error) << "No GDALSystem registered";
		return NULL;
	}

	OGRFeature * f =  _currentSys->createFeature(*this);
	this->newFeatures_write.push_back(f);
	return f;
}

//OGRFeatureShadow *ViewContainer::createFeatureShadow()
//{
//	return (OGRFeatureShadow*)(this->createFeature());
//}

void ViewContainer::syncAlteredFeatures()
{
	if (!_currentSys) {
		Logger(Error) << "No GDALSystem registered";
		return;
	}

	this->_currentSys->syncAlteredFeatures(*this, this->dirtyFeatures_write);
	this->_currentSys->syncNewFeatures(*this, this->newFeatures_write);

}

void ViewContainer::syncReadFeatures()
{
	foreach (OGRFeature *f, this->dirtyFeatures_read) {
		OGRFeature::DestroyFeature(f);
	}
	this->dirtyFeatures_read.clear();
}

OGRFeature *ViewContainer::getOGRFeature(long nFID)
{
	if (!_currentSys) {
		Logger(Error) << "No GDALSystem registered";
		return NULL;
	}

	OGRFeature * f =  this->_currentSys->getOGRLayer(*this)->GetFeature(nFID);
	registerFeature(f);

	return f;

}

void ViewContainer::registerFeature(OGRFeature * f)
{
	if (!f)
		return;
	if (readonly) {
		this->dirtyFeatures_read.push_back(f);
		return;
	}
	this->dirtyFeatures_write.push_back(f);
	return;
}


OGRFeature *ViewContainer::getFeature(long dynamind_id)
{
	OGRFeature * f = this->_currentSys->getFeature(*this, dynamind_id);
	if (!f)
		return NULL;
	if (readonly) {
		this->dirtyFeatures_read.push_back(f);
		return f;
	}
	registerFeature(f);
	return f;
}

OGRFeature *ViewContainer::getNextFeature()
{
	if (!_currentSys) {
		Logger(Error) << "No GDALSystem registered";
		return NULL;
	}

	OGRFeature * f =  this->_currentSys->getNextFeature(*this);
	registerFeature(f);
	return f;
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
