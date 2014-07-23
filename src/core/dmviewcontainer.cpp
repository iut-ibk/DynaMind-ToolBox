/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2014  Christian Urich

 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

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

string ViewContainer::getDBID()
{
	return this->_currentSys->getDBID();
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
		//Logger(Error) << "No GDALSystem registered";
		return;
	}

	this->_currentSys->syncAlteredFeatures(*this, this->dirtyFeatures_write);
	this->_currentSys->syncNewFeatures(*this, this->new_Features_write_not_owned, false);
	this->_currentSys->syncNewFeatures(*this, this->newFeatures_write, false);

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


/**OGRFeature *ViewContainer::getFeature(long dynamind_id)
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
}**/

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

OGRFeatureDefnShadow * DM::ViewContainer::getFeatureDef()
{
	return (OGRFeatureDefnShadow *) this->_currentSys->getOGRLayer(*this)->GetLayerDefn();
}

void ViewContainer::registerFeature(OGRFeatureShadow *f)
{
	OGRFeature * feature = (OGRFeature*) f;
	std::string t(feature->GetDefnRef()->GetFieldDefn(0)->GetNameRef());
	this->_currentSys->registerFeature(feature, *this);
	this->new_Features_write_not_owned.push_back(feature);
}


}
