/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @author  Michael Mair <abroxos@gmail.com>
 * @version 1.0
 * @section LICENSE
 * This file is part of DynaMind
 *
 * Copyright (C) 2011-2012  Christian Urich, Michael Mair, Markus Sengthaler

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
#include <QtCore>

using namespace std;

namespace DM {

struct LinkAttribute {
    std::string viewname;
    std::string uuid;
};

class Component;

/** @ingroup DynaMind-Core
  * An Attribute is used to add informations to an object.
  *
  * As attributes Double, Striunb DoubleVector and StringVectors can be used.
  */
class DM_HELPER_DLL_EXPORT Attribute
{
public:
    enum AttributeType {
        NOTYPE,
        DOUBLE,
        STRING,
        TIMESERIES,
        LINK,
        DOUBLEVECTOR,
        STRINGVECTOR
    };
    
private:
	std::string _uuid;	// this one is really unique
    std::string name;
    std::set<std::string> inViews;

	void SQLInsertThis(AttributeType type);
	void SQLDeleteThis();
    void SQLUpdateValue(AttributeType type, QVariant value);
    //void SQLSetOwner(Component* owner);
	void SQLSetName(std::string newname);
	void SQLSetType(AttributeType newtype);
    bool SQLGetValue(QVariant &value) const;
	void SQLSetValue(AttributeType type, QVariant value);

    void setValue(QByteArray bytes);
    //QByteArray getValue(AttributeType type) const;
protected:
public:
    /** @brief copies type and value to this attribute**/
    void Change(Attribute &attribute);
    /** @brief changes the owner **/
	void SetOwner(Component* owner);
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
    /** @brief creates a new string attribute**/
    Attribute(std::string name, std::string val);
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
    /** @brief destructor */
    ~Attribute();
    /** @brief return datatype*/
    AttributeType getType() const;
    /** @brief add link object **/
    void setLink(std::string viewname, std::string uuid);
    /** @brief Sets attribute links the existing vector is cleared! **/
    void setLinks(std::vector<LinkAttribute> links);
    /** @brief Returns the first element in the link attribute vector. If no link exists it retruns an empty LinkAttribute */
    LinkAttribute getLink();
    /** @brief Returns Vector of Links */
    std::vector<LinkAttribute> getLinks();
    /** @brief add TimeSeries **/
    void addTimeSeries(std::vector<std::string> timestamp, std::vector<double> value);
    /** @brief add TimeSeries **/
    void getTimeSeries(std::vector<std::string> *timestamp, std::vector<double> *value);
    /** @brief Sets attribute type */
    void setType(AttributeType type);
    /**
     * @brief get a printable name of the type, e.g. for gui displaying
     * @return the name of the type as string
     */
    const char *getTypeName() const;
    
    /**
     * @brief convert @AttributeType to a printabel character e.g. for gui displaying
     * @param type the requested type
     * @return the name of the @arg type
     */
    static const char*getTypeName(AttributeType type);
};
typedef std::map<std::string, DM::Attribute*> AttributeMap;
}
#endif // ATTRIBUTE_H
