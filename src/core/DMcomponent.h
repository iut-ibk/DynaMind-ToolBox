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

#ifndef COMPONENT_H
#define COMPONENT_H

#include <iostream>
#include <map>
#include <vector>
#include <list>
#include <set>
#include <DMview.h>
#include <compilersettings.h>

namespace DM {

class Attribute;

class DM_HELPER_DLL_EXPORT Component
{
    friend class System;
protected:
    std::string name;
    std::map<std::string,Component*> childsview;
    std::map<std::string,Attribute*> attributesview;
    std::map<std::string,Component*> ownedchilds;
    std::map<std::string,Attribute*> ownedattributes;
    std::set<std::string> inViews;

    void removeView(const DM::View & view);


public:
    Component();
    Component(const Component& s);
    virtual ~Component();
    void setName(std::string name);
    std::string getName();
    bool addAttribute(Attribute newattribute);
    bool addAttribute(std::string, double val);
    bool changeAttribute(Attribute newattribute);
    bool removeAttribute(std::string name);
    Attribute* getAttribute(std::string name);
    std::map<std::string, Attribute*> getAllAttributes();
    bool addChild(Component *newcomponent);
    bool changeChild(Component *newcomponent);
    bool removeChild(std::string name);
    Component* getChild(std::string name);
    std::map<std::string, Component*> getAllChilds();
    void setView(std::string view);
    void setView(const DM::View & view);
    void createNewUUID();
    std::set<std::string> const &  getInViews() const;
    bool isInView(DM::View view) const;
    virtual Component* clone()=0;

};
}
#endif // COMPONENT_H
