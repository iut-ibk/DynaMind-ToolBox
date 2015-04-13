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

#ifndef DMGDALSYSTEM_H
#define DMGDALSYSTEM_H


#include <dmviewcontainer.h>
#include <dmisystem.h>
#include <ogr_api.h>
#include <dmlogger.h>

#include <map>
#include <vector>

#ifdef SWIG
#define DM_HELPER_DLL_EXPORT
#endif

class OGRDataSource;
class OGRSFDriver;
class OGRLayer;
class OGRFeature;

namespace DM {
/**
 * @brief The GDALSystem class provides a data stream based on GDAL library.
 *
 * Data are stored in a Spatilite DB. The database can be found in the tmp directory of the system. As filename
 * an uuid is created. For each of the views a seperate table is created. Data in the DB can be directly opend with common GIS programs
 * such as QGIS supporting Spatilite databases.
 *
 * The GDALSystem class itself is used only internaly and the provided methods should not be accessed by the module developer.To
 * access, modify and create features use the ViewContainer class.
 *
 * The GDALSystem centrally manages all Features accessed, modified and created data for each view.This means the commonly used
 * Destroy Feature used when sunig the GDAL library should not be called!
 *
 */
class DM_HELPER_DLL_EXPORT GDALSystem : public ISystem
{
public:
	/**
	 * @brief Constructor for GDALSystem
	 * @param EPSG
	 */
	GDALSystem(int EPSG = 0);

	/**
	 * @brief Init GDAL System with Database
	 * @param EPSG
	 */
	void setGDALDatabase(const string &database);

	/**
	 * @brief Creates a new GDALSystem.
	 *
	 * To create a new GDAL the DB the source DB is copies and a new uuid is assigend to the new DB.
	 * The method is used to create successor states of the GDALSystem.
	 * @param Successor of the GDALSystem
	 */
	GDALSystem(const GDALSystem& s);

	/**
	 * @brief Syncronises modified Features for a given view with the DB. The method destroyes the features.
	 * @param v: View
	 * @param df: Vector of modified Features
	 * @param destroy:If true method destroy features (default for features created in C++) If flase destroy is not called
	 * (default for features created in Python)
	 */
	void syncAlteredFeatures(const DM::View & v, std::vector<OGRFeature *> & df, bool destroy);

	/**
	 * @brief Syncronises newly created Features for a given view with the DB.
	 * @param v: View
	 * @param df: Vector of modified Features
	 * @param destroy:If true method destroy features (default for features created in C++) If flase destroy is not called
	 * (default for features created in Python)
	 *
	 */
	void syncNewFeatures(const DM::View & v, std::vector<OGRFeature *> & df, bool destroy);

	void synsDeleteFeatures(const DM::View & v, std::vector<long> & df);

	/**
	 * @brief Updates view in system.
	 *
	 * Updates the layer dfinition used as table dfinition in the DB.
	 */
	void updateView(const View &v);

	/**
	 * @brief Unused leftover from System class
	 */
	std::vector<Component*> getAllComponentsInView(const DM::View &view)
	{
		DM::Logger(DM::Error) << "getAllComponentsInView not implemented";
		return std::vector<Component*>();
	}

	/**
	 * @brief See update View
	 */
	void updateViewContainer(DM::ViewContainer v);

	/**
	 * @brief Creates a new OGRFeature
	 */
	OGRFeature *createFeature(const DM::View & v);

	/**
	 * @brief Returns OGRLayer for a given view
	 */
	OGRLayer *getOGRLayer(const DM::View & v);

	/**
	 * @brief Returns OGRDataSource (the Spatilite DB)
	 */
	OGRDataSource *getDataSource();

	/**
	 * @brief Resets reader for givesn view
	 */
	bool resetReading(const DM::View & v);

	GDALSystem *createSuccessor();

	//OGRFeature * getFeature(const DM::View & v, long dynamind_id);

	/**
	 * @brief Updates a vector of view (see updateView)
	 */
	void updateViews(const std::vector<View> &views);

	/**
	 * @brief Return predecessor
	 */
	GDALSystem *getPredecessor() const;
	/**
	 * @brief Sets predecessor
	 */
	void setPredecessor(GDALSystem *sys);

	/**
	 * @brief Returns next feature in sequential reading
	 */
	OGRFeature *getNextFeature(const DM::View & v);

	/**
	 * @brief Returns current state ID (check if still needed?)
	 */
	std::string getCurrentStateID();

	std::string getDBID();

private:
	OGRDataSource						*poDS;
	OGRSFDriver							*poDrive;
	std::map<std::string, OGRLayer *>	viewLayer;
	std::vector<std::string>			state_ids;

	OGRLayer *createLayer(const View &v);
	GDALSystem *predecessor;
	std::vector<DM::GDALSystem*> sucessors;
	QString DBID;
	int EPSG;
};
}

#endif // DMGDALSYSTEM_H
