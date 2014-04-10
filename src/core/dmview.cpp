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

#include "dmview.h"
#include <dmmodule.h>
#include <dmattribute.h>
#include <dmcomponent.h>

using namespace DM;

typedef std::map<std::string, std::string> stringmap;

View::View(std::string name, int type, ACCESS accesstypeGeometry):
	name(name), type(type), accesstypeGeometry(accesstypeGeometry)
{
}

View::View() 
{
	this->name = "";
	this->type = -1;
}

View View::clone(ACCESS forceAccessType, const std::string& newName) const
{
	View v(newName.empty()?name:newName, type, forceAccessType);
	for(std::map<std::string, AttributeAccess>::iterator it = v.attributes.begin(); 
		it != v.attributes.end(); ++it)
	{
		it->second.access = forceAccessType;
	}
	return v;
}

void View::addAttribute(const std::string name, Attribute::AttributeType type, ACCESS access)
{
	AttributeAccess& aa = attributes[name];
	aa.type = type;
	aa.access = access;
}

void View::addAttribute(const std::string name, std::string linkName, ACCESS access)
{
	AttributeAccess& aa = attributes[name];
	aa.type = Attribute::LINK;
	aa.access = access;
	aa.linkedView = linkName;
}

bool View::reads() const
{
	if (this->accesstypeGeometry < WRITE)
		return true;

	mforeach(const AttributeAccess& aa, attributes)
		if(aa.access < WRITE)
			return true;

	return false;
}

bool View::writes() const
{
	if (this->accesstypeGeometry > READ)
		return true;

	mforeach(const AttributeAccess& aa, attributes)
		if(aa.access > READ)
			return true;

	return false;
}

Attribute::AttributeType View::getAttributeType(std::string name) const
{
	AttributeAccess aa;
	if(map_contains(&attributes, name, aa))
		return aa.type;
	return Attribute::NOTYPE;
}

ACCESS View::getAttributeAccessType(std::string name) const
{
	AttributeAccess aa;
	if(map_contains(&attributes, name, aa))
		return aa.access;
	return READ;
}

std::vector<std::string> View::getAllAttributes() const
{
	std::vector<std::string> names;
	for (std::map<std::string, AttributeAccess>::const_iterator it = attributes.begin(); it != attributes.end(); ++it)
		names.push_back(it->first);
	return names;
}

bool View::hasAttribute(const std::string& attrName) const
{
	if (map_contains(&attributes, attrName))
		return true;
	else
		return false;
}

std::string View::getNameOfLinkedView(string linkName) const
{
	AttributeAccess aa;
	if(map_contains(&attributes, linkName, aa))
		return aa.linkedView;
	return "";
}

void View::addFilter(const std::string& condition)
{
	filter = condition;
}

std::string View::getFilter() const
{
	return filter;
}
