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

#ifndef DMATTRIBUTE_H
#define DMATTRIBUTE_H

#include <iostream>
#include <vector>
#include <set>
#include <dmcompilersettings.h>
using namespace std;


namespace DM {
    class DM_HELPER_DLL_EXPORT Attribute
    {
    private:
        std::string name;
        double doublevalue;
        std::string stringvalue;
        std::vector<double> doublevector;
        std::vector<std::string> stringvector;
        std::set<std::string> inViews;

    public:
        Attribute(const Attribute &newattribute);
        Attribute(std::string name);
        Attribute(std::string name, double val);
        Attribute(){}
        ~Attribute();
        void setDouble(double v);
        double getDouble();
        void setString(std::string s);
        std::string getString();
        void setDoubleVector(std::vector<double> v);
        std::vector<double> getDoubleVector();
        void setStringVector(std::vector<std::string> s);
        std::vector<std::string> getStringVector();
        void setName(std::string name);
        std::string getName();
    };
}
#endif // ATTRIBUTE_H
