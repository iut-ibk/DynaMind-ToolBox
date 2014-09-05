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

#ifndef VIEWCONTAINER_H
#define VIEWCONTAINER_H

#include <dmview.h>

#ifdef SWIG
#define DM_HELPER_DLL_EXPORT
#endif

class DM_HELPER_DLL_EXPORT OGRFeatureShadow;
class DM_HELPER_DLL_EXPORT OGRFeature;
class DM_HELPER_DLL_EXPORT OGRFeatureDefnShadow;
class DM_HELPER_DLL_EXPORT OGRGeometry;
class DM_HELPER_DLL_EXPORT OGRPoint;

namespace DM {

class DM_HELPER_DLL_EXPORT GDALSystem;


/**
 * @brief The ViewContainer class is used by to manage data access to the GDALSystem. This includes reading,
 * creating and modifiying features.
 *
 * The class is derived form the View. This means to define the view. In particular
 * View attributes use the method addAttribute from the View class.
*/
class DM_HELPER_DLL_EXPORT ViewContainer : public View
{
public:
	/** @brief Default constructor **/
	ViewContainer();

	/**
	 * @brief Constructor to initialse ViewContainer.
	 * The View container mainly defines the name and access to its geometry.
	 * To access are add attributes please use the inherited addAttribute method
	 *
	 * @param name: Name of the View accessed
	 * @param type: Supported types are NODE, EDGE or FACE
	 * @param assesstypeGeometry: This can either be READ, MODIFY or WRITE
	 *
	*/
	ViewContainer(std::string name, int type, ACCESS accesstypeGeometry = READ);

	/**
	 * @brief Sets the pointer to the current system. This should not be used by
	 * the module developer. The Pointer to the current system is set by the simulaton class
	 * @param sys: Current system, set by the simulation class
	 */
	void setCurrentGDALSystem(DM::GDALSystem * sys);

	/**
	 * @brief Creates a new feature.
	 *
	 * The feature is managed by the GDALSystem and should not be destroyed.
	 * All newly created features are syncronised after the module has been executed.
	 * For details about the OGRFeature please take a look a the GDAL documentation under hhtp://www.gdal.org
	 * @return new OGRFeature
	 */
	OGRFeature* createFeature();

	/**
	 * @brief Syncronise altered features with the database.
	 *
	 * Method writes alter features in the database and destroyes the object.
	 */
	void syncAlteredFeatures();
	/**
	 * @brief Syncronise read features.
	 *
	 * Destroyes the accessed OGRFeatures
	 */
	void syncReadFeatures();

	/**
	 * @brief Returns the number of features
	 */
	int getFeatureCount();

	/**
	 * @brief Sets the position of the read marker to the beginning.
	 */
	bool resetReading();
	/**
	 * @brief Returns OGRFeature with the nFID
	 * @param nFID feature ID
	 */
	OGRFeature *getFeature(long nFID);


	//OGRFeature *getFeature(long dynamind_id);

	/**
	 * @brief Retruns the pointer the the next feature.
	 *
	 * This is used to sequentially read all features within the view container.
	 * @code
	 * OGRFeature *poFeature;
	 * while( (poFeature = viewcontainer.getNextFeature()) != NULL ) {
	 * 	//Iterates over features in container until end
	 * }
	 * @endcode
	 *
	 * @return
	 */
	OGRFeature *getNextFeature();

	void createIndex(std::string attribute);

	/**
	 * @brief Returns OGRFeatureDefinition. Only used internally to allow python wrapping
	 */
	OGRFeatureDefnShadow * getFeatureDef();

	/**
	 * @brief Registeres a Feature generated in Python. Only used internally to allow python wrapping
	 */
	void registerFeature(OGRFeatureShadow * f, bool isNew);

	/**
	 * @brief ~ViewContainer
	 */
	virtual ~ViewContainer();

	std::string getDBID();

	/**
	 * @brief Set attribute filter. For doc please see GDAL API
	 * @param filter
	 */
	void setAttributeFilter(std::string filter);

	/**
	 * @brief Set spatail filter. For doc please see GDAL API
	 * @param filter
	 */
	void setSpatialFilter(OGRGeometry * geo);

	void createSpatialIndex();

	void deleteFeature(long id);

	OGRFeature *findNearestPoint(OGRPoint * p, double radius = 1.0);



private:
	GDALSystem * _currentSys;//Pointer to System, updated by simulation
	std::vector<OGRFeature *> newFeatures_write; //Container for objects generated within C++
	std::vector<OGRFeature *> new_Features_write_not_owned; //Container for object generated in Python (couldn't work out how to transfer the ownership)
	std::vector<OGRFeature *> dirtyFeatures_write_not_owned; //Container for object generated in Python (couldn't work out how to transfer the ownership)
	std::vector<OGRFeature *> dirtyFeatures_write;
	std::vector<OGRFeature *> dirtyFeatures_read;

	std::vector<long> delete_ids;

	bool readonly;
	void registerFeature(OGRFeature * f);
};
}

#endif // VIEWCONTAINER_H
