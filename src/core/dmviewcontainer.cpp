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
#include <sstream>
#include <iomanip>
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

void ViewContainer::setAttributeFilter(string filter)
{
	if (!_currentSys) {
		Logger(Error) << "No GDALSystem registered";
		return;
	}

	OGRLayer * lyr = this->_currentSys->getOGRLayer((*this));
	lyr->SetAttributeFilter(filter.c_str());
}

void ViewContainer::setSpatialFilter(OGRGeometry *geo)
{
	if (!_currentSys) {
		Logger(Error) << "No GDALSystem registered";
		return;
	}

	OGRLayer * lyr = this->_currentSys->getOGRLayer((*this));
	lyr->SetSpatialFilter(geo);
}

void ViewContainer::createSpatialIndex()
{
	std::stringstream index;
	index << "SELECT CreateSpatialIndex('" << this->getName() << "','GEOMETRY')";
	OGRLayer * lyr = this->_currentSys->getDataSource()->ExecuteSQL(index.str().c_str(), 0, "SQLITE");

}

void ViewContainer::deleteFeature(long id)
{
	if (!_currentSys) {
		Logger(Error) << "No GDALSystem registered";
		return;
	}
	delete_ids.push_back(id);
}

OGRFeature * ViewContainer::findNearestPoint(OGRPoint * p, double radius)
{
	std::stringstream query;
	query << std::setprecision(15);
	query << "SELECT OGC_FID as id, ST_Distance(Geometry, MakePoint("
		  << p->getX() << "," << p->getY() <<")) AS Distance FROM "<< this->getName() << " WHERE distance < " << radius <<" ORDER BY distance LIMIT 1";
	OGRLayer * lyr = this->_currentSys->getDataSource()->ExecuteSQL(query.str().c_str(), 0, "SQLITE");
	if (!lyr) {
		Logger(Error) << "find nearest point query failed: " << query.str().c_str();
		return NULL;
	}
	OGRFeature * f = 0;

	while (f = lyr->GetNextFeature()) {
		int id =  f->GetFieldAsInteger("id");
		this->_currentSys->getDataSource()->ReleaseResultSet(lyr);
		return this->getFeature(id);
	}
	this->_currentSys->getDataSource()->ReleaseResultSet(lyr);
	return NULL;
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

	this->setAttributeFilter("");
	this->setSpatialFilter(0);

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

void ViewContainer::syncAlteredFeatures()
{
	if (!_currentSys) {
		return;
	}

	this->_currentSys->syncAlteredFeatures(*this, this->dirtyFeatures_write, true);
	this->_currentSys->syncAlteredFeatures(*this, this->dirtyFeatures_write_not_owned, false);

	this->_currentSys->syncNewFeatures(*this, this->new_Features_write_not_owned, false);
	this->_currentSys->syncNewFeatures(*this, this->newFeatures_write, true);

	this->_currentSys->synsDeleteFeatures(*this, this->delete_ids);

}

void ViewContainer::syncReadFeatures()
{
	foreach (OGRFeature *f, this->dirtyFeatures_read) {
		OGRFeature::DestroyFeature(f);
	}
	this->dirtyFeatures_read.clear();
}

OGRFeature *ViewContainer::getFeature(long nFID)
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

void ViewContainer::createIndex(string attribute)
{
	std::stringstream index_drop;
	index_drop << "DROP INDEX "<< attribute <<"_index";
	OGRLayer * lyr = this->_currentSys->getDataSource()->ExecuteSQL(index_drop.str().c_str(), 0, "SQLITE");

	std::stringstream index;
	index << "CREATE INDEX "<< attribute <<"_index ON " << this->getName() << " (" << attribute << ")";
	lyr = this->_currentSys->getDataSource()->ExecuteSQL(index.str().c_str(), 0, "SQLITE");
}

int ViewContainer::getFeatureCount()
{
	if (!_currentSys) {
		Logger(Error) << "No GDALSystem registered";
		return -1;
	}
	return this->_currentSys->getOGRLayer(*this)->GetFeatureCount();
}

bool ViewContainer::resetReading()
{
	if (!_currentSys) {
		Logger(Error) << "No GDALSystem registered";
		return false;
	}
	return this->_currentSys->resetReading(*this);
}

OGRFeatureDefnShadow * DM::ViewContainer::getFeatureDef()
{
	return (OGRFeatureDefnShadow *) this->_currentSys->getOGRLayer(*this)->GetLayerDefn();
}

void ViewContainer::registerFeature(OGRFeatureShadow *f, bool isNew)
{
	if (this->readonly)
		return;
	OGRFeature * feature = (OGRFeature*) f;

	if (isNew) {
		this->_currentSys->registerFeature(feature, *this);
		this->new_Features_write_not_owned.push_back(feature);
	} else {
		this->dirtyFeatures_write_not_owned.push_back(feature);
	}

}
}
