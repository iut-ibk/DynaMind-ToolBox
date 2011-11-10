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

#include "vectordata.h"
#include <iostream>
#include <cstdlib>
#include <QThread>
#include <vibe_logger.h>
#include <math.h>
#include <DMnode.h>
using vibens::Debug;
using vibens::Error;

using vibens::Logger;

bool Point::operator ==(const Point & other) const {

    return this->x == other.getX() && this->y == other.getY() && this->z == other.getZ();
}
Point Point::operator -(const Point & other) const {
    return(Point(this->x - other.getX(), this->y - other.getY(), this->z - other.getZ()));
}

Point Point::operator +(const Point & other) const {
    return(Point(this->x + other.getX(), this->y + other.getY(), this->z + other.getZ()));
}

Point Point::operator *(const Point & other) const {
    return(Point(this->x * other.getX(), this->y * other.getY(), this->z * other.getZ()));
}
Point Point::operator /(const Point & other) const {
    return(Point(this->x / other.getX(), this->y / other.getY(), this->z / other.getZ()));
}
bool Point::compare2d(const Point &other, double round ) const {

    return abs( this->x - other.getX() ) <= round   &&  abs( this->y - other.getY() ) <= round;
}

double Attribute::getAttribute(const std::string & name) const
{
    if (Attributes.find(name) != Attributes.end()) {
        return  QString::fromStdString(Attributes.at(name)).toDouble();
    }
    return 0;
}

std::string Attribute::getStringAttribute(const std::string & name) const
{
    if (Attributes.find(name) != Attributes.end()) {
        return  Attributes.at(name);
    }
    return "";

}




void Attribute::setAttribute(std::string name, double value) {
    std::stringstream ss;
    ss << value;
    this->Attributes[name] = ss.str();
}

void Attribute::setAttribute(std::string name, std::string value) {
    this->Attributes[name] = value;
}

std::vector<std::string> Attribute::getAttributeNames() {
    std::vector<std::string> names;
    for (boost::unordered_map<std::string, std::string>::iterator it = Attributes.begin(); it != Attributes.end(); ++it)
        names.push_back(it->first);
    return names;
}

VectorData::VectorData() {
    this->system = new DM::System("VectorData", "Root");

}

VectorData::VectorData(DM::System *system)
{
    this->system = system;
}

VectorData::~VectorData()
{
    //delete this->system;
}

void VectorData::addVectorData(const VectorData &vec) {
    foreach(std::string n , vec.getAttributeNames()) {
        this->setAttributes(n, vec.getAttributes(n));
    }
    foreach(std::string n , vec.getEdgeNames()) {
        this->setEdges(n, vec.getEdges(n));
    }
    foreach(std::string n , vec.getPointsNames()) {
        this->setPoints(n, vec.getPoints(n));
    }
    foreach(std::string n , vec.getFaceNames()) {
        this->setFaces(n, vec.getFaces(n));
    }
    foreach(std::string n , vec.getLinkNames()) {
        this->setLinks(n, vec.getLinks(n));
    }
}

void VectorData::setPoints(const std::string & name, const std::vector<Point> & points) {



    stringstream ids;
    ids << "Point*|*"<< name << "*|*";

    for (int i = 0; i < points.size(); i++) {
        stringstream id;
        id <<  ids.str() << i;
        system->addNode(new DM::Node( id.str(),"ID", points[i].x,  points[i].y, points[i].z));
    }



}

void VectorData::setDoubleAttributes(const std::string & name, const std::vector<double> & attributes) {
    
    this->vDoubleAttributes[name] = attributes;
}

void VectorData::setEdges(const std::string & name, const std::vector<Edge> & edges) {
    boost::unordered_map<std::string, std::vector<Edge> >::iterator n = this->vEdges.find(name);
    if (n == vEdges.end())
        this->EdgeNames.push_back(name);    
    this->vEdges[name] = edges;
}

void VectorData::setFaces(const std::string & name, const std::vector<Face> & faces) {
    boost::unordered_map<std::string, std::vector<Face> >::iterator n = this->vFaces.find(name);
    if (n == vFaces.end())
        this->FaceNames.push_back(name);    
    this->vFaces[name] = faces;
}

void VectorData::setLinks(const std::string & name, const std::vector<Link> & faces) {
    boost::unordered_map<std::string, std::vector< Link > >::iterator n = this->vLinks.find(name);
    if (n == this->vLinks.end())
        this->LinkNames.push_back(name);    
    this->vLinks[name] = faces;
}

void VectorData::setAttributes(const std::string & name, const Attribute & attributes) {
    boost::unordered_map<std::string, Attribute >::iterator n = this->vAttributes.find(name);
    if (n == vAttributes.end())
        this->AttributeNames.push_back(name);
    this->vAttributes[name] = attributes;

}


const std::vector<Point>  & VectorData::getPoints(const std::string & name) const{


    if (vData.find(name) != vData.end())
        return  vData.at(name);
    return errorPointAttribute;
}

const std::vector<double>  & VectorData::getDoubleAttributes(const std::string & name) const {
    if (vDoubleAttributes.find(name) != vDoubleAttributes.end()) {
        return vDoubleAttributes.at(name);
    }
    return errorDoubleAttribute;
}
const Attribute  & VectorData::getAttributes(const std::string & name) const {
    if (vAttributes.find(name) != vAttributes.end()) {
        return vAttributes.at(name);
    }
    return errorAttribute;

}

const std::vector<Edge> & VectorData::getEdges(const std::string & name) const{
    if (vEdges.find(name) != vEdges.end()) {
        return vEdges.at(name);
    }
    return errorEdgeAttribute;
}

const std::vector<Face> & VectorData::getFaces(const std::string & name) const {
    if (vFaces.find(name) != vFaces.end()) {
        return vFaces.at(name);
    }
    return errorFaceAttribute;
}

const std::vector<Link> & VectorData::getLinks(const std::string & name) const {
    if (vLinks.find(name) != vLinks.end()) {
        return vLinks.at(name);
    }
    return errorLinkAttribute;
}
void VectorData::clear() {
    vData.clear();
    vEdges.clear();
    vFaces.clear();
    vLinks.clear();
    vDoubleAttributes.clear();
    vAttributes.clear();


    VectorDataNames.clear();
    EdgeNames.clear();
    FaceNames.clear();
    LinkNames.clear();
    AttributeNames.clear();
}

std::ostream& operator<<(std::ostream& out, const VectorData& vec) {
    out << "<VectorData>" << std::endl;
    out << "\t"<< "\t" << "<Name value=\""
        << vec.getName() << "\"/>" << "\n";
    out << "\t"<< "\t" << "<UUID value=\""
        << vec.getUUID() << "\"/>" << "\n";


    out << "</VectorData>" << std::endl;


    return out;

}

const std::vector<std::string> VectorData::getPointsNames() const {
    std::map<std::string, DM::Node*> nodes = this->system->getAllNodes();
    std::vector<std::string> names;
    for (std::map<std::string, DM::Node*>::const_iterator it = nodes.begin(); it != nodes.end(); ++it) {
        names.push_back(it->first);
    }

    return names;
}
