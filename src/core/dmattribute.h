/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @author  Michael Mair <abroxos@gmail.com>
 * @version 1.0
 * @section LICENSE
 * This file is part of DynaMind
 *
 * Copyright (C) 2011-2012  Christian Urich, Michael Mair

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
#include <map>
#include <dmcompilersettings.h>
using namespace std;

/** @ingroup DynaMind-Core
  * An Attribute is used to add informations to an object.
  *
  * As attributes Double, Striunb DoubleVector and StringVectors can be used.
  */
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

    bool isDouble;
    bool isString;
    bool isDoubleVector;
    bool isStringVector;

public:
    /** @brief Returns true if a double value is set **/
    bool hasDouble();
    /** @brief Returns true if a string value is set **/
    bool hasString();
    /** @brief Returns true if a double vector is set **/
    bool hasDoubleVector();
    /** @brief Returns true if a string vector is set **/
    bool hasStringVector();
    /** @brief creates a new attribute with the values from an existing attribute **/
    Attribute(const Attribute &newattribute);
    /** @brief creates a new named attribute**/
    Attribute(std::string name);
    /** @brief creates a new double attribute**/
    Attribute(std::string name, double val);
    /** @brief creates a new attribute**/
    Attribute();
    /** @brief set double value**/
    void setDouble(double v);
    /** @brief get double value**/
    double getDouble();
    /** @brief set string value**/
    void setString(std::string s);
    /** @brief get string value**/
    std::string getString();
    /** @brief set double vector**/
    void setDoubleVector(std::vector<double> v);
    /** @brief get double vector**/
    std::vector<double> getDoubleVector();
    /** @brief set string vector**/
    void setStringVector(std::vector<std::string> s);
    /** @brief get string vector**/
    std::vector<std::string> getStringVector();
    /** @brief set attribute name */
    void setName(std::string name);
    /** @brief get name */
    std::string getName();
    ~Attribute();
};
typedef std::map<std::string, DM::Attribute*> AttributeMap;
}
#endif // ATTRIBUTE_H
