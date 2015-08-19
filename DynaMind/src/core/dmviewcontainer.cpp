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

std::string ViewContainer::get_attribute_filter_sql_string(string filter)
{
	std::stringstream filter_string;
	for (int i = 0; i < this->moduleLevelFilter.size(); i++) {
		if (i < 0) {
			filter_string << "AND ";
		}
		filter_string << moduleLevelFilter[i] << " ";
	}

	if ( !filter.empty()) {
		if (this->moduleLevelFilter.size() > 0 )
			filter_string << "AND ";
		filter_string << filter;
	}

	return filter_string.str();
}

void ViewContainer::setAttributeFilter(string filter)
{
	if (!_currentSys) {
		Logger(Error) << "No GDALSystem registered";
		return;
	}

	OGRLayer * lyr = this->_currentSys->getOGRLayer((*this));

	lyr->SetAttributeFilter(get_attribute_filter_sql_string(filter).c_str());
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
	/*query << "SELECT OGC_FID as id, ST_Distance(Geometry, MakePoint("
		  << p->getX() << "," << p->getY() <<")) AS Distance FROM "<< this->getName() << " WHERE distance < " << radius
		 << " AND ST_Contains(ST_Expand(MakePoint(" << p->getX() << "," << p->getY() <<"),200),Geometry)" <<"  ORDER BY distance LIMIT 1";*/
	query << "SELECT OGC_FID as id, "
		  << "ST_Distance(Geometry, MakePoint("
		  << p->getX() << "," << p->getY() <<")) AS distance FROM "<< this->getName()
		  << " WHERE ROWID IN (SELECT ROWID FROM SpatialIndex WHERE f_table_name = '" << this->getName() << "'"
		  << "AND search_frame = BuildCircleMbr("<< p->getX() << ","<< p->getY() << "," << radius << "))  ORDER BY distance ASC LIMIT 1";

	//DM::Logger(DM::Error) << query.str();
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

void ViewContainer::addModuleAttributeFilter(string filter)
{
	this->moduleLevelFilter.clear();
	this->moduleLevelFilter.push_back(filter);

	this->setAttributeFilter("");
}

OGRLayer *ViewContainer::executeSQL(string query)
{
	return this->_currentSys->getDataSource()->ExecuteSQL(query.c_str(), 0, "SQLITE");
}

void ViewContainer::linkFeatures(OGRFeature *f1, OGRFeature *f2, string link_name)
{
	//Default link name is view name f2_id
	if (link_name.empty()) {
		std::stringstream link_ss;
		link_ss << f2->GetDefnRef()->GetName() << "_id";
		link_name = link_ss.str();
	}
	f1->SetField(link_name.c_str(), (int) f2->GetFID());

}

std::vector<OGRFeature *> ViewContainer::getLinkedFeatures(OGRFeature *f, string link_name)
{
	//Default link name is view name f2_id
	if (link_name.empty()) {
		std::stringstream link_ss;
		link_ss <<  f->GetDefnRef()->GetName() << "_id";
		link_name = link_ss.str();
	}
	std::vector<OGRFeature *> linked_features;

	std::stringstream filter;
	filter << link_name << " = " << f->GetFID();
	this->resetReading();
	this->setAttributeFilter(filter.str());

	OGRFeature * f_linked;
	while(f_linked = this->getNextFeature()) {
		linked_features.push_back(f_linked);
	}

	return linked_features;

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

void ViewContainer::setNextByIndex(long index){
	if (!_currentSys) {
		Logger(Error) << "No GDALSystem registered";
	}

	this->_currentSys->setNextByIndex(*this, index);
}

void ViewContainer::createIndex(string attribute)
{
	std::stringstream index_drop;
	index_drop << "DROP INDEX IF EXISTS "<< attribute <<"_index";
	this->_currentSys->getDataSource()->ExecuteSQL(index_drop.str().c_str(), 0, "SQLITE");

	std::stringstream index;
	index << "CREATE INDEX "<< attribute <<"_index ON " << this->getName() << " (" << attribute << ")";
	this->_currentSys->getDataSource()->ExecuteSQL(index.str().c_str(), 0, "SQLITE");
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
		this->new_Features_write_not_owned.push_back(feature);
	} else {
		this->dirtyFeatures_write_not_owned.push_back(feature);
	}

}


}
