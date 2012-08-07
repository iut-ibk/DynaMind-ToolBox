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

#include <dmmodule.h>
#include <dmcomponent.h>
#include <dmattribute.h>
#include <QVariant>

using namespace DM;

Attribute::Attribute(const Attribute &newattribute)
{

    this->name=newattribute.name;
    this->doublevalue=newattribute.doublevalue;
    this->stringvalue=newattribute.stringvalue;
    this->doublevector=newattribute.doublevector;
    this->stringvector=newattribute.stringvector;
    this->type = newattribute.type;
}

Attribute::Attribute(std::string name)
{
    this->name=name;
    this->type = NOTYPE;
}
Attribute::Attribute()
{
    this->name="";
    this->type = NOTYPE;

}

Attribute::Attribute(std::string name, double val)
{
    this->doublevalue = val;
    this->name=name;

    this->type = DOUBLE;

}

Attribute::~Attribute()
{
}

int Attribute::getType()
{
    return type;
}

void Attribute::setName(std::string name)
{
    this->name=name;
}


std::string Attribute::getName()
{
    return name;
}


void Attribute::setDouble(double v)
{
    doublevalue=v;
    this->type = DOUBLE;
}

double Attribute::getDouble()
{
    return doublevalue;
}

void Attribute::setString(std::string s)
{
    stringvalue=s;
    this->type = STRING;
}

std::string Attribute::getString()
{
    return stringvalue;
}

void Attribute::setDoubleVector(std::vector<double> v)
{
    doublevector=v;
    this->type = DOUBLEVECTOR;
}

std::vector<double> Attribute::getDoubleVector()
{
    return doublevector;
}

void Attribute::setStringVector(std::vector<std::string> s)
{
    stringvector=s;
    this->type = STRINGVECTOR;
}

std::vector<std::string> Attribute::getStringVector()
{
    return stringvector;
}

bool Attribute::hasDouble()
{
    if (type == DOUBLE)
        return true;
    return false;
}
bool Attribute::hasDoubleVector()
{
    if (type == DOUBLEVECTOR)
        return true;
    return false;
}
bool Attribute::hasString()
{
    if (type == STRING)
        return true;
    return false;
}
bool Attribute::hasStringVector()
{
    if (type == STRINGVECTOR)
        return true;
    return false;
}

void Attribute::setLink(string viewname, string uuid)
{
    this->type = LINK;
    this->stringvector.push_back(viewname);
    this->stringvector.push_back(uuid);
}

LinkAttribute Attribute::getLink()
{
    LinkAttribute attr;
    attr.viewname = this->stringvector[0];
    attr.uuid = this->stringvector[1];
    return attr;

}

void Attribute::addTimeSeries(std::vector<std::string> timestamp, std::vector<double> value)
{
    this->type = TIMESERIES;
    this->doublevector = value;
    this->stringvector = timestamp;
}

void Attribute::setType(int type)
{
    this->type = type;
}
