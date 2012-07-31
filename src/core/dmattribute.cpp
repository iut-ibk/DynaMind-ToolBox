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

    this->isDouble = newattribute.isDouble;
    this->isString = newattribute.isString;
    this->isDoubleVector = newattribute.isDoubleVector;
    this->isStringVector = newattribute.isStringVector;
}

Attribute::Attribute(std::string name)
{
    this->name=name;
    this->doublevalue = 0;
    this->stringvalue = "";
    this->doublevector = std::vector<double>();
    this->stringvector = std::vector<string>();


}
Attribute::Attribute()
{

}

Attribute::Attribute(std::string name, double val)
{
    this->doublevalue = val;
    this->name=name;

    isDouble = true;
    isString = false;
    isDoubleVector = false;
    isStringVector = false;

}

Attribute::~Attribute()
{
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
    isDouble = true;
}

double Attribute::getDouble()
{
    return doublevalue;
}

void Attribute::setString(std::string s)
{
    stringvalue=s;
    isString = true;
}

std::string Attribute::getString()
{
    return stringvalue;
}

void Attribute::setDoubleVector(std::vector<double> v)
{
    doublevector=v;
    isDoubleVector = true;
}

std::vector<double> Attribute::getDoubleVector()
{
    return doublevector;
}

void Attribute::setStringVector(std::vector<std::string> s)
{
    stringvector=s;
    isStringVector = true;
}

std::vector<std::string> Attribute::getStringVector()
{
    return stringvector;
}

bool Attribute::hasDouble()
{
   return this->isDouble;
}
bool Attribute::hasDoubleVector()
{
   return this->isDoubleVector;
}
bool Attribute::hasString()
{
   return this->isString;
}
bool Attribute::hasStringVector()
{
   return this->isStringVector;
}

