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

namespace DM {

    class Attribute;

    class Component
    {
    protected:
        std::string name;
        std::string id;
        std::map<std::string,Component*> childsview;
        std::map<std::string,Attribute*> attributesview;
        std::map<std::string,Component*> ownedchilds;
        std::map<std::string,Attribute*> ownedattributes;

    public:
        Component(std::string name, std::string id);
        Component(const Component& s);
        virtual ~Component();
        void setName(std::string name);
        std::string getName();
        void setID(std::string id);
        std::string getID();
        bool addAttribute(Attribute *newattribute);
        bool changeAttribute(Attribute *newattribute);
        bool removeAttribute(std::string name);
        Attribute* getAttribute(std::string name);
        std::map<std::string, Attribute*> getAllAttributes();
        bool addChild(Component *newcomponent);
        bool changeChild(Component *newcomponent);
        bool removeChild(std::string name);
        Component* getChild(std::string name);
        std::map<std::string, Component*> getAllChilds();
        virtual Component* clone()=0;
    };
}
#endif // COMPONENT_H
