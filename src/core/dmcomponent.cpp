/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @author  Michael Mair <abroxos@gmail.com>
 * @version 1.0
 * @section LICENSE
 * This file is part of DynaMind
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
#include <dmsystem.h>
#include <assert.h>
#include <dmlogger.h>

#include <QUuid>

using namespace DM;
using namespace DM;


Component::Component()
{
    this->uuid = QUuid::createUuid().toString().toStdString();
    name = "";
    childsview = std::map<std::string,Component*>();
    attributesview = std::map<std::string,Attribute*>();
    ownedchilds = std::map<std::string,Component*>();
    ownedattributes =  std::map<std::string,Attribute*>();
    inViews = std::set<std::string>();
    currentSys = 0;

}

void Component::createNewUUID() {
    this->uuid = QUuid::createUuid().toString().toStdString();
}

bool Component::isInView(View view) const {
    foreach (std::string s, inViews) {
        if (view.getName().compare(s) == 0)
            return true;
    }

    return false;
}

void Component::setName(std::string name) {
    this->name = name;
}

//std::string Component::getName() const {
//return name;
//}

Component::Component(const Component& c)
{

    this->uuid=c.uuid;
    attributesview=c.attributesview;
    ownedchilds=c.ownedchilds;
    inViews = c.inViews;
    std::map<std::string,Component*>::iterator it;

    for ( it=ownedchilds.begin() ; it != ownedchilds.end(); it++ )
    {

        childsview[(*it).first]=ownedchilds[(*it).first];
    }
    ownedchilds.clear();

}
Component * Component::updateChild(Component * c) {
    if (ownedchilds.find(c->getUUID()) != ownedchilds.end())
        return c;
    Component * c_new = c->clone();
    changeChild(c_new);

    return c_new;
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

void Component::setUUID(std::string uuid)
{
    this->uuid=uuid;
}

std::string Component::getUUID()
{
    return uuid;
}

bool Component::addAttribute(std::string name, double val) {
    if(attributesview.find(name)!=attributesview.end()) {
        return this->changeAttribute(name, val);
    }
    Attribute  attr = Attribute(name, val);
    return this->addAttribute(attr);
}

bool Component::addAttribute(std::string name, std::string val) {
    if(attributesview.find(name)!=attributesview.end()) {
        return this->changeAttribute(name, val);
    }
    Attribute  attr = Attribute(name, val);
    return this->addAttribute(attr);
}

bool Component::addAttribute(Attribute newattribute)
{
    if(attributesview.find(newattribute.getName())!=attributesview.end())
        return this->changeAttribute(newattribute);
    Attribute * a = new Attribute(newattribute);
    attributesview[newattribute.getName()] = a;
    ownedattributes[newattribute.getName()] = a;

    return true;
}

bool Component::changeAttribute(Attribute newattribute)
{

    if(attributesview.find(newattribute.getName())==attributesview.end()) {
        return this->addAttribute(newattribute);
    }
    if(ownedattributes.find(newattribute.getName())==ownedattributes.end()) {
        ownedattributes[newattribute.getName()] = new Attribute(*(attributesview[newattribute.getName()]));
        attributesview[newattribute.getName()] = ownedattributes[newattribute.getName()];
    }
    Attribute * attr = attributesview[newattribute.getName()];
    Attribute::AttributeType type = attr->getType();
    attr->setDouble(newattribute.getDouble());
    attr->setDoubleVector(newattribute.getDoubleVector());
    attr->setString(newattribute.getString());
    attr->setStringVector(newattribute.getStringVector());
    attr->setType(type);
    return true;
}

bool Component::changeAttribute(std::string s, double val)
{
    return this->changeAttribute(Attribute(s, val));
}

bool Component::changeAttribute(std::string s, std::string val)
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

    if(childsview.find(newcomponent->getUUID())!=childsview.end())
        return false;

    childsview[newcomponent->getUUID()] = newcomponent;
    ownedchilds[newcomponent->getUUID()] = newcomponent;


    return true;
}
Component* Component::clone() {
    return new Component(*this);
}

bool Component::changeChild(Component *newcomponent)
{
    if(!newcomponent)
        return false;

    if(ownedchilds.find(newcomponent->getUUID())!=ownedchilds.end())
        delete ownedchilds[newcomponent->getUUID()];

    ownedchilds[newcomponent->getUUID()] = newcomponent;
    childsview[newcomponent->getUUID()] = newcomponent;

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

System * Component::getCurrentSystem() {
    return this->currentSys;
}

void Component::setCurrentSystem(System *sys) {
    this->currentSys = sys;
}

