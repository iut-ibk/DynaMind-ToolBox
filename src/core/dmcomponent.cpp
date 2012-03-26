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

#include <QtCore>
#include <dmcomponent.h>
#include <dmattribute.h>

#include <assert.h>
#include <dmlogger.h>

#include <QUuid>

using namespace DM;
using namespace DM;


Component::Component()
{
    this->name = QUuid::createUuid().toString().toStdString();
}

void Component::createNewUUID() {
    this->name = QUuid::createUuid().toString().toStdString();
}

bool Component::isInView(View view) const {
    foreach (std::string s, inViews) {
        if (view.getName().compare(s) == 0)
            return true;
    }

    return false;
}

Component::Component(const Component& c)
{
    this->name=c.name;
    attributesview=c.attributesview;
    ownedchilds=c.ownedchilds;
    inViews = c.inViews;

    std::map<std::string,Component*>::iterator it;

    for ( it=ownedchilds.begin() ; it != ownedchilds.end(); it++ )
    {
        ownedchilds[(*it).first]=(*it).second->clone();
        childsview[(*it).first]=ownedchilds[(*it).first];
    }


}

Component::~Component()
{
    while(ownedchilds.size())
    {
        delete (*ownedchilds.begin()).second;
        ownedchilds.erase(ownedchilds.begin());
    }

    while(ownedattributes.size())
    {
        delete (*ownedattributes.begin()).second;
        ownedattributes.erase(ownedattributes.begin());
    }
}

void Component::setName(std::string name)
{
    this->name=name;
}

std::string Component::getName()
{
    return name;
}

bool Component::addAttribute(std::string name, double val) {
    if(attributesview.find(name)!=attributesview.end()) {
        return this->changeAttribute(name, val);
    }
    Attribute  attr = Attribute(name, val);
    return this->addAttribute(attr);

}

bool Component::addAttribute(Attribute newattribute)
{
    if(attributesview.find(newattribute.getName())!=attributesview.end())
        return false;
    Attribute * a = new Attribute(newattribute);
    attributesview[newattribute.getName()] = a;
    ownedattributes[newattribute.getName()] = a;
    return true;
}

bool Component::changeAttribute(Attribute newattribute)
{
    /*if(ownedattributes.find(newattribute.getName())!=ownedattributes.end())
        delete ownedattributes[newattribute.getName()];
    Attribute * b = new Attribute(newattribute);
    ownedattributes[newattribute.getName()] = b;
    attributesview[newattribute.getName()] = b;*/

    if(attributesview.find(newattribute.getName())==attributesview.end()) {
        return this->addAttribute(newattribute);
    }
    Attribute * attr = attributesview[newattribute.getName()];
    attr->setDouble(newattribute.getDouble());
    attr->setDoubleVector(newattribute.getDoubleVector());
    attr->setString(newattribute.getString());
    attr->setStringVector(newattribute.getStringVector());

    return true;
}

bool Component::changeAttribute(std::string s, double val)
{
    return this->changeAttribute(Attribute(s, val));
}


bool Component::removeAttribute(std::string name)
{
    if(attributesview.find(name)!=attributesview.end())
    {
        if(ownedattributes.find(name)!=ownedattributes.end())
        {
            delete ownedattributes[name];
            ownedattributes.erase(name);
        }

        attributesview.erase(name);
        return true;
    }

    return false;
}

Attribute* Component::getAttribute(std::string name)
{
    if(attributesview.find(name)==attributesview.end()) {
        this->addAttribute(Attribute(name));
    }
    return attributesview[name];
}

const std::map<std::string, Attribute*> & Component::getAllAttributes() const
{
    return attributesview;
}

bool Component::addChild(Component *newcomponent)
{
    if(!newcomponent)
        return false;

    if(childsview.find(newcomponent->getName())!=childsview.end())
        return false;

    childsview[newcomponent->getName()] = newcomponent;
    ownedchilds[newcomponent->getName()] = newcomponent;
    return true;
}

bool Component::changeChild(Component *newcomponent)
{
    if(!newcomponent)
        return false;

    if(ownedchilds.find(newcomponent->getName())!=ownedchilds.end())
        delete ownedchilds[newcomponent->getName()];

    ownedchilds[newcomponent->getName()] = newcomponent;
    childsview[newcomponent->getName()] = newcomponent;

    return true;
}

bool Component::removeChild(std::string name)
{
    if(childsview.find(name)!=childsview.end())
    {
        if(ownedchilds.find(name)!=ownedchilds.end())
        {
            delete ownedchilds[name];
            ownedchilds.erase(name);
        }

        childsview.erase(name);
        return true;
    }

    return false;
}

Component* Component::getChild(std::string name)
{
    if(childsview.find(name)==childsview.end())
        return 0;
    return childsview[name];
}

void Component::setView(std::string view)
{
    this->inViews.insert(view);
}

void Component::setView(const DM::View & view)
{
    this->inViews.insert(view.getName());
}

void Component::removeView(const View &view)
{
    this->inViews.erase(view.getName());
}

const set<std::string> &Component::getInViews() const {
    return this->inViews;

}
std::map<std::string, Component*> Component::getAllChilds()
{
    return childsview;
}
