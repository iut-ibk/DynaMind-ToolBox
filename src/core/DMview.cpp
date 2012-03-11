/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @author  Michael Mair <abroxos@gmail.com>
 * @version 1.0
 * @section LICENSE
 * This file is part of DynaMite
 *
 * Copyright (C) 2011  Christian Urich, Michael Mair

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


#include "DMview.h"

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
  *
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
  * To every View a Dummy Component is created. The dummy component is used to track the changes made by the modules.
  * @author Christian Urich
  * @date 11.03.2012
  */

namespace DM {
View::View(std::string name, int type, int accesstypeGeometry)
{
    this->name = name;
    this->type = type;
    this->accesstypeGeometry = accesstypeGeometry;

}
View::View() {
    this->name = "";
    this->type = -1;
}

void View::setIdOfDummyComponent(std::string UUID) {
    this->IdofDummyComponent = UUID;
}

std::string View::getIdOfDummyComponent() {
    return this->IdofDummyComponent;
}

void View::addAttribute(std::string name) {
    this->ownedAttributes[name] = WRITE;
}

void View::getAttribute(std::string name) {
    this->ownedAttributes[name] = READ;
}
void View::modifyAttribute(std::string name) {
    this->ownedAttributes[name] = MODIFY;
}

std::vector<std::string> View::getWriteAttributes() const {
    std::vector<std::string> attrs;
    for (std::map<std::string, int>::const_iterator it = this->ownedAttributes.begin(); it != this->ownedAttributes.end(); ++it) {
        if (it->second > READ)
            attrs.push_back(it->first);
    }

    return attrs;
}

std::vector<std::string> View::getReadAttributes() const {
    std::vector<std::string> attrs;
    for (std::map<std::string, int>::const_iterator it = this->ownedAttributes.begin(); it != this->ownedAttributes.end(); ++it) {
        if (it->second < WRITE)
            attrs.push_back(it->first);
    }

    return attrs;



}

/**
  * @brief Returns true if the accesstype of the geomtry or from one attribute is modify or read
  *
  */
bool View::reads() {
    if (this->accesstypeGeometry < WRITE)
        return true;
    for (std::map<std::string, int>::const_iterator it = this->ownedAttributes.begin(); it != this->ownedAttributes.end(); ++it) {
        if (it->second < WRITE)
            return true;
    }

    return false;
}

/**
  * @brief Returns true if the accesstype of the geomtry or from one attribute is modify or write
  *
  */
bool View::writes() {
    if (this->accesstypeGeometry > READ)
        return true;
    for (std::map<std::string, int>::const_iterator it = this->ownedAttributes.begin(); it != this->ownedAttributes.end(); ++it) {
        if (it->second > READ)
            return true;
    }

    return false;
}

bool View::operator<(const View & other) const {
         if (this->getName().compare(other.getName()) < 0)
             return true;
         return false;
    }
}


