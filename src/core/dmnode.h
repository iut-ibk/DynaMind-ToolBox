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

#ifndef DMNODE_H
#define DMNODE_H

#include <dmcompilersettings.h>
#include "dmdbconnector.h"

#ifdef SWIG
#define DM_HELPER_DLL_EXPORT
#endif

#include <string>

namespace DM {
class Component;
class Vector3;
class Edge;
/**
     * @ingroup DynaMind-Core
     * @brief Provides a 3D node object
     *
     * A node is defined by x, y and z. Edge and Face components just contain reference
     * to the node objects.
     * Nodes are derived from the Component class. Therefore nodes are identified by an UUID and can hold an
     * unlimeted number of Attributes.
     */
class DM_HELPER_DLL_EXPORT Node : public Component
{
private:
    Vector3* vector;
    bool    isInserted;
    /** @brief return table name */
    QString getTableName();
	
	std::vector<Edge*> *connectedEdges;	// not cached, for now
	void addEdge(Edge* e)
	{
		if(!connectedEdges)
			connectedEdges = new std::vector<Edge*>();
		connectedEdges->push_back(e);
	}
	friend class Edge;
protected:
    virtual void SetOwner(Component *owner);
public:
    /** @brief create new Node object defined by x, y and z */
    Node( double x, double y, double z );
    /** @brief create empty Node object. x, y and z are 0 */
    Node();
    /** @brief destructor*/
    ~Node();
    /** @brief creates a copy of the node including its components (UUID, Attributes,...)*/
    Node(const Node& n);
	/** @brief return Type */
	Components getType();
    /** @brief return x */
    double getX() const;
    /** @brief return y*/
    double getY() const;
    /** @brief return z*/
    double getZ() const;
    /** @brief return coordinates */
    const double get(unsigned int i) const;
    /** @brief writes a xyz-double[3] into the given vector */
    const void get(double *vector) const;
    /** @brief returns all edges connecting this node */
    std::vector<Edge*> getEdges() const;
    /** @brief set x*/
    void setX(double x);
    /** @brief set y*/
    void setY(double y);
    /** @brief set z*/
    void setZ(double z);
    /** @brief set xyz*/
    void set(double x, double y, double z);
	/** @brief =operator */
	Node& operator=(Node const& other);
    /** @brief return true if x, y and z are the same*/
    bool operator==(const Node & other) const;
    /** @brief x1-x2, y1-y2, z1-z2*/
    Node operator-(const Node & other) const;
    /** @brief x1+x2, y1+y2, z1+z2*/
    Node operator+(const Node & other) const;
    /** @brief x1*val, y1*val, z1*val*/
    Node operator*(const double & val) const;
    /** @brief x1/val, y1/val, z1/val*/
    Node operator/(const double & val) const;
    /** @brief return if x and y are +- round the same*/
    bool compare2d(const Node &other, double round = 0) const;
    /** @brief return if x and y are +- round the same*/
    bool compare2d(const Node * other, double round = 0) const;
    /** @brief  Creates a pointer to a cloned Node object, including Attributes and uuid*/
    Component* clone();

#ifdef CACHE_PROFILING
    static void PrintStatistics();
#endif
    Vector3* LoadFromDb();
    void SaveToDb(Vector3* v);
};

class Vector3
{
public:
    double x,y,z;

    Vector3(){}
    Vector3(double x,double y,double z){this->x=x;this->y=y;this->z=z;}
    Vector3(const Vector3 &ref){this->x=ref.x;this->y=ref.y;this->z=ref.z;}
};

typedef std::map<std::string, DM::Node*> NodeMap;
}
#endif // NODE_H
