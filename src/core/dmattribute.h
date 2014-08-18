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
#include "dmdbconnector.h"
#include <dmdbcache.h>

using namespace std;

namespace DM {

/*class LinkAttribute 
{
public:
	LinkAttribute() {}
	LinkAttribute(std::string viewname, std::string uuid) : viewname(viewname) , uuid(uuid){}
	std::string viewname;
	std::string uuid;

	bool operator==(const LinkAttribute & other) const {
		return this->uuid == other.uuid && this->viewname == other.viewname;
	}
};*/

class TimeSeriesAttribute
{
public:
	TimeSeriesAttribute(const std::vector<std::string>& timestamp, const std::vector<double>& value):
		timestamp(timestamp), value(value)
	{
	}

	TimeSeriesAttribute(){};
	std::vector<std::string> timestamp;
	std::vector<double> value;
};

class Component;
class System;

/**
* An Attribute is used to add informations to an object.
*
* As attributes Double, Striunb DoubleVector and StringVectors can be used.
*/
class DM_HELPER_DLL_EXPORT Attribute
{
public:
	enum AttributeType 
	{
		NOTYPE,
		DOUBLE,
		STRING,
		TIMESERIES,
		LINK,
		DOUBLEVECTOR,
		STRINGVECTOR,
		INT
	};
	class AttributeValue
	{
	public:
		AttributeValue();
		AttributeValue(const AttributeValue& ref);
		AttributeValue(double d);
		AttributeValue(std::string string);
		~AttributeValue();
		void Free();
		AttributeValue(QVariant var, AttributeType type, System* owningSystem);
		QVariant toQVariant();
		
		Attribute::AttributeType type;
		void*	ptr;
	};

	/** @brief =operator */
	Attribute& operator=(Attribute const& other);

	/** @brief copies type and value to this attribute**/
	void Change(const Attribute &attribute);

	/** @brief changes the owner **/
	void setOwner(Component* owner);

	/** @brief returns the current owner, be aware on successor state generated attributes:
	their owner is the component owning the element, not just the pointer **/
	Component* GetOwner();

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
	Attribute(const std::string& name, AttributeType type);

	/** @brief creates a new double attribute**/
	Attribute(const std::string& name, double val);

	/** @brief creates a new string attribute**/
	Attribute(const std::string& name, std::string val);

	/** @brief creates a new attribute**/
	Attribute();

	/** @brief set double value**/
	void setDouble(double v);

	/** @brief get double value**/
	double getDouble();

	/** @brief set string value**/
	void setString(const std::string& s);

	/** @brief get string value**/
	std::string getString();

	/** @brief set double vector**/
	void setDoubleVector(const std::vector<double>& v);

	/** @brief get double vector**/
	std::vector<double> getDoubleVector();

	/** @brief set string vector**/
	void setStringVector(const std::vector<std::string>& s);

	/** @brief get string vector**/
	std::vector<std::string> getStringVector();

	/** @brief set attribute name */
	void setName(const std::string& name);

	/** @brief get name */
	std::string getName() const;

	/** @brief destructor */
	~Attribute();

	/** @brief return datatype*/
	AttributeType getType() const;

	/** @brief add link object **/
	void addLink(Component* target, const std::string& viewName);

	/** @brief remove a specific link from this attribute **/
	bool removeLink(Component* target);

	/** @brief clear all links **/
	void clearLinks();

	/** @brief add link object **/
	std::vector<Component*> getLinkedComponents();

	/** @brief add TimeSeries **/
	void addTimeSeries(const std::vector<std::string>& timestamp, const std::vector<double>& value);

	/** @brief add TimeSeries **/
	void getTimeSeries(std::vector<std::string> *timestamp, std::vector<double> *value);

	/** @brief Sets attribute type */
	void setType(AttributeType type);

	/**
	* @brief get a printable name of the type, e.g. for gui displaying
	* @return the name of the type as string
	*/
	const char* getTypeName() const;

	/**
	* @brief convert @AttributeType to a printabel character e.g. for gui displaying
	* @param type the requested type
	* @return the name of the @arg type
	*/
	static const char* getTypeName(AttributeType type);

	static void _MoveAttribute(Attribute* a);
	static Attribute* _createAttribute(const std::string& attributeName, Component* owner, 
		const QVariant& value, AttributeType type, System* currentSystem);

private:
	std::string		name;
	Component*		owner;
	AttributeValue	value;
	bool			isInserted;
};
typedef std::map<std::string, DM::Attribute*> AttributeMap;
}
#endif // ATTRIBUTE_H
