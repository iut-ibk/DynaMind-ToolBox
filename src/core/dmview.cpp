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

View::View(std::string name, int type, int accesstypeGeometry)
{
	this->name = name;
	this->type = type;
	this->accesstypeGeometry = accesstypeGeometry;

}
View::View() 
{
	this->name = "";
	this->type = -1;
}
/*
void View::addAttribute(std::string name) {
	this->ownedAttributes[name] = WRITE;
	this->attributeTypes[name] = Attribute::NOTYPE;
}

void View::getAttribute(std::string name) {
	this->ownedAttributes[name] = READ;
	this->attributeTypes[name] = Attribute::NOTYPE;
}
void View::modifyAttribute(std::string name) {
	this->ownedAttributes[name] = MODIFY;
	this->attributeTypes[name] = Attribute::NOTYPE;
}*/

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
/*
std::vector<std::string> View::getWriteAttributes() const {
	std::vector<std::string> attrs;
	for (std::map<std::string, int>::const_iterator it = this->ownedAttributes.begin(); it != this->ownedAttributes.end(); ++it)
		if (it->second > READ)
			attrs.push_back(it->first);

	return attrs;
}

std::vector<std::string> View::getReadAttributes() const {
	std::vector<std::string> attrs;
	for (std::map<std::string, int>::const_iterator it = this->ownedAttributes.begin(); it != this->ownedAttributes.end(); ++it)
		if (it->second < WRITE)
			attrs.push_back(it->first);

	return attrs;
}*/

bool View::reads() const
{
	if (this->accesstypeGeometry < WRITE)
		return true;
	//for (std::map<std::string, int>::const_iterator it = this->ownedAttributes.begin(); it != this->ownedAttributes.end(); ++it)
	//	if (it->second < WRITE)
	mforeach(const AttributeAccess& aa, attributes)
		if(aa.access < WRITE)
			return true;

	return false;
}


bool View::writes() const
{
	if (this->accesstypeGeometry > READ)
		return true;
	//for (std::map<std::string, int>::const_iterator it = this->ownedAttributes.begin(); it != this->ownedAttributes.end(); ++it)
	//	if (it->second > READ)
	mforeach(const AttributeAccess& aa, attributes)
		if(aa.access > READ)
			return true;

	return false;
}

Attribute::AttributeType View::getAttributeType(std::string name) const
{
	//std::map<std::string, Attribute::AttributeType>::const_iterator it = attributeTypes.find(name);
	//if(it != attributeTypes.end())
	//	return it->second;
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

/*
void View::setAttributeType(std::string name, Attribute::AttributeType type)
{
	this->attributeTypes[name] = type;
}*/
/*
void View::addLinks(string name, View linkedView)
{
	addLinks(name, linkedView.getName());
}

void View::addLinks(string name, std::string linkedViewName)
{
	this->addAttribute(name);
	this->setAttributeType(name, Attribute::LINK);
	this->attributeLinks[name] = linkedViewName;
}*/
/*
std::vector<std::string> View::getNamesOfLinks()
{
	std::vector<std::string> namesOfView;
	for (stringmap::const_iterator it = attributeLinks.begin(); it != attributeLinks.end(); ++it) {
		namesOfView.push_back(it->first);
	}
	return namesOfView;

}*/

std::string View::getNameOfLinkedView(string linkName) const
{
	//std::string viewName;
	//map_contains(&attributeLinks, linkName, viewName);
	//return viewName;
	AttributeAccess aa;
	if(map_contains(&attributes, linkName, aa))
		return aa.linkedView;
	return "";
}


