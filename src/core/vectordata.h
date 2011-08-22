/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of VIBe2
 *
 * Copyright (C) 2011  Christian Urich

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

#ifndef VECTORDATA_H
#define VECTORDATA_H
/**
  * @addtogroup Datatypes
  * @brief VIBe2 Datatypes
  * @author Christian Urich
  */

#include "compilersettings.h"
#include <map>
#include <string>
#include <vector>
#include <boost/unordered_map.hpp>

class VIBE_HELPER_DLL_EXPORT Point {
public:
    Point(){}
    Point(double x, double y, double z){this->x = x; this->y = y; this->z = z;}
    double x;
    double y;
    double z;
    double  getX() const {return x;}
    double  getY() const {return y;}
    double  getZ() const{return z;}
    bool operator==(const Point & other) const;
    Point operator-(const Point & other) const;
    Point operator+(const Point & other) const;
    Point operator*(const Point & other) const;
    Point operator/(const Point & other) const;
    bool compare2d(const Point &other, double round = 0) const;

};
class VIBE_HELPER_DLL_EXPORT Edge {
public:
    Edge(){}
    Edge(long id1, long id2){this->id1 = id1; this->id2 = id2;}
    long  id1;
    long  id2;
    long  getID1() const {return id1;}
    long  getID2() const {return id2;}

};

class VIBE_HELPER_DLL_EXPORT Face {
public:
    Face(){}
    Face(std::vector<long> ids){this->ids = ids;}
    std::vector<long> ids;
    std::vector<long> getIDs()const {return this->ids;}

};

class VIBE_HELPER_DLL_EXPORT Link {
public:
    Link(){}
    Link(std::string VectorDataName, std::string  ID){ this->VectorDataName = VectorDataName; this->ID = ID;}
    std::string VectorDataName;
    std::string ID;

    std::string getVectorDataName(){return this->VectorDataName;}
    std::string getID(){return this->ID;}

};

class VIBE_HELPER_DLL_EXPORT Attribute {
public:
    Attribute(){}
    boost::unordered_map<std::string,std::string> Attributes;
    double getAttribute(const std::string & name) const;
    std::string getStringAttribute(const std::string & name) const;
    void setAttribute(std::string name, double value);
    void setAttribute(std::string name, std::string value);
    std::vector<std::string> getAttributeNames();

    // For Python things
    void setAttribute_double(std::string name, double value) {
        this->setAttribute(name, value);
    }

    void setAttribute_string(std::string name, std::string value) {
        this->setAttribute(name, value);
    }

};

/** @brief VIBe2 Datatype VectorData
  *
  * The Vectordata type is used to store all kinds of geographic inforamtions. Therefore the consits of
  * - Points
  * - Edges
  * - Faces
  * - Links
  * - Attributes
  *
  * Complex data structures can be stored in the VectorData datatype. Following data structures are used in DAnCE4Water and PowerVIBe.
  * Gridbased
  *
  * Parcel based
  *
  * UrbanEnvironment:
  *
  * - Attribute \b Globals: Contains global informations
  *     - Datatype = 'ParcelBased'
  *     - Year [INT]
  *
  * @ingroup Datatypes
  * @author Christian Urich
  */
class VIBE_HELPER_DLL_EXPORT VectorData
{

private:
    boost::unordered_map<std::string, std::vector<Point> > vData;
    boost::unordered_map<std::string, std::vector<Edge> > vEdges;
    boost::unordered_map<std::string, std::vector<Face> > vFaces;
    boost::unordered_map<std::string, std::vector< Link > > vLinks;
    boost::unordered_map<std::string, std::vector<double> > vDoubleAttributes;
    boost::unordered_map<std::string, Attribute > vAttributes;


    std::vector<std::string> VectorDataNames;
    std::vector<std::string> EdgeNames;
    std::vector<std::string> FaceNames;
    std::vector<std::string> LinkNames;
    std::vector<std::string> AttributeNames;

    std::string name;
    std::string uuid;

    Attribute errorAttribute;
    std::vector<double> errorDoubleAttribute;
    std::vector<Point> errorPointAttribute;
    std::vector<Edge> errorEdgeAttribute;
    std::vector<Face> errorFaceAttribute;
    std::vector<Link> errorLinkAttribute;

public:
    void setName(std::string name) {this->name = name;}
    std::string getName() const {return this->name;}

    void setUUID(std::string UUID){this->uuid = UUID;}
    std::string getUUID()const {return this->uuid;}

    void setPoints(const std::string &name, const std::vector<Point> &points);
    void setDoubleAttributes(const std::string &name, const std::vector<double> &attributes);    
    void setAttributes(const std::string &name, const Attribute &attributes);
    void setEdges(const std::string & name, const std::vector<Edge> & edges);
    void setFaces(const std::string & name, const std::vector<Face> & faces);
    void setLinks(const std::string & name, const std::vector<Link> & links);
    void addVectorData(const VectorData & vec);
    void clear();

    const std::vector<Point>  & getPoints(const std::string &name) const;
    const std::vector<double> &  getDoubleAttributes(const std::string &name) const;
    const Attribute & getAttributes(const std::string &name) const;
    const std::vector<Edge> & getEdges(const std::string &name) const;
    const std::vector<Face> & getFaces(const std::string &name) const;
    const std::vector<Link> & getLinks(const std::string &name) const;


    const std::vector<std::string> & getPointsNames() const {return VectorDataNames;}
    const std::vector<std::string> & getEdgeNames() const {return EdgeNames;}
    const std::vector<std::string> & getFaceNames() const {return FaceNames;}
    const std::vector<std::string> & getLinkNames() const {return LinkNames;}
    const std::vector<std::string> & getAttributeNames() const {return AttributeNames;}




    VectorData();
    ~VectorData();


};

std::ostream& operator<<(std::ostream& out, const VectorData& p);

#endif // VECTORDATA_H
