/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @author  Michael Mair <abroxos@gmail.com>
 * @author  Markus Sengthaler <m.sengthaler@gmail.com>
 * @version 1.0
 * @section LICENSE
 * This file is part of DynaMite
 *
 * Copyright (C) 2011  Christian Urich, Michael Mair, Markus Sengthaler

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

#ifndef DMVIEW_H
#define DMVIEW_H

#include <map>
#include <vector>
#include <string>
#include <dmcompilersettings.h>
#include <dmattribute.h>

namespace DM {

enum ACCESS
{
	READ = 0,
	MODIFY = 1,
	WRITE = 2,
	DEL = 3
};

/**
  * @class DM::View
  *
  * @brief Data in DynaMind are organised in views. A view provides a description of a
  * component and its attributes and how they are accessed in the module.
  *
  * @ingroup DynaMind-Core
  *
  *
  * A view is defined by:
  *
  * @section Type
  * Following Type are supported in DynaMind:
  * -   COMPONENT
  * -   NODE
  * -   EDGE
  * -   FACE
  * -   SUBSYSTEM
  * -   RASTERDATA
  *
  * @section AccessType
  * The AccessType defines the assecctype geometric component.
  * Following AccessTypes are supported:
  * - Read: Geometry stays the same and no new components are added to the view
  * - Write: Gemetroy of existing components can be changed and new components can be added to the view
  * - Modify: Gemeotry can be modified but no new components are added to the view
  *
  * @section Attributes
  * Every component can hold attributes. In the view the access to the attributes of the component is defined.
  * Following methods can be used to access attributes within a module
  *
  * - addAttribute(std::string name)
  * - getAttribute(std::string name)
  * - modifyAttribute(std::string name)
  *
  * @section Dummy Component
  * To every View a dummy component is created. The dummy component is used to track the changes made by the modules.
  * @author Christian Urich
  * @date 11.03.2012
  */
class DM_HELPER_DLL_EXPORT View
{
public:
	/** @brief Default constructor to create a new view */
	View(std::string name, int type, ACCESS accesstypeGeometry = READ);
	View();
	
	/** @brief clone the view, forcing a new access type and name */
	View clone(ACCESS forceAccessType, const std::string& newName = "") const;
	
	/** @brief add attributes to be accessed with this view */
	void addAttribute(const std::string name, Attribute::AttributeType type, ACCESS access);

	/** @brief Adds link attribute */
	void addAttribute(const std::string name, std::string linkName, ACCESS access);

	/** @brief Get name of the View*/
	std::string const & getName() const {return this->name;}

	/** @brief Return Type */
	int const & getType() const {return type;}

	/** @brief Return AccessType of the Geometry */
	ACCESS getAccessType() const {return accesstypeGeometry;}

	/** @brief Returns true if the accesstype of the geomtry or from one attribute is modify or read */
	bool reads() const;

	/** @brief Returns true if the accesstype of the geomtry or from one attribute is modify or write */
	bool writes() const;

	/** @brief Returns attribute type */
	Attribute::AttributeType getAttributeType(std::string name) const;
	
	/** @brief Returns attribute access type */
	ACCESS getAttributeAccessType(std::string name) const;

	/** @brief Sets Attribute Type */
	//void setAttributeType(std::string name, Attribute::AttributeType type);

	/** @brief Returns names of object the linked view */
	std::string getNameOfLinkedView(std::string linkName) const;
	
	/** @brief Returns names of all attributes */
	std::vector<std::string> getAllAttributes() const;

	bool hasAttribute(const std::string& attrName) const;

	void addFilter(const std::string& condition);

	std::string getFilter() const;
private:
	int type;
	std::string name;
	ACCESS accesstypeGeometry;
	std::string	filter;

	struct AttributeAccess
	{
		Attribute::AttributeType type;
		ACCESS access;
		std::string linkedView;
	};

	std::map<std::string, AttributeAccess> attributes;
};
}


#endif // DMVIEW_H
