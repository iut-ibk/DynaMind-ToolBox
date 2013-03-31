/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2012  Christian Urich

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

#include "dmgeometry.h"


namespace DM {

void SpatialNodeHashMap::addNodeToSpatialNodeHashMap(DM::Node *n)
{
    QString key = this->spatialHashNode(n->getX(),n->getY());
    std::vector<DM::Node* > * nodes = (*this)[key];
    if (!nodes) {
        nodes = new std::vector<DM::Node* >;
        (*this)[key] = nodes;
    }
    nodes->push_back(n);
}

QString SpatialNodeHashMap::spatialHashNode(const double & x, const double  & y)
{
    int ix = (int) x / devider;
    int iy = (int) y / devider;
    QString key = QString::number(ix) + "|" +  QString::number(iy);
    return key;
}

Node *SpatialNodeHashMap::findNode(const double &x, const double &y, const double &tol)
{
    DM::Node n_tmp(x,y,0);
    QString key = this->spatialHashNode(x,y);
    std::vector<DM::Node* > * nodes = (*this)[key];
    if (!nodes) {
        return 0;
    }

    foreach (DM::Node * n, *nodes) {
        if (n->compare2d(&n_tmp, tol))
            return n;
    }
    return 0;
}

Node * SpatialNodeHashMap::addNode(double x, double y, double z, double tol, View v)
{
    DM::Node * n = this->findNode(x, y, tol);
    if (n) {
        if (n->isInView(v) ) return n;
        else sys->addComponentToView(n,v); return n;
    }

    n = sys->addNode(x,y,z,v);
    this->addNodeToSpatialNodeHashMap(n);
    return n;
}

void SpatialNodeHashMap::addNodesFromView(const DM::View &view)
{
    std::vector<std::string> uuid_nodes = sys->getUUIDs(view);
    foreach (std::string uuid, uuid_nodes)
        this->addNodeToSpatialNodeHashMap(sys->getNode(uuid));
}

SpatialNodeHashMap::SpatialNodeHashMap(DM::System * sys, double devider, bool init, const DM::View & nodeView) :  devider(devider), sys(sys)
{
    if (!init)
        return;
    if (nodeView.getName().empty()) {
        this->addNodesFromView(nodeView);
        return;
    }
    NodeMap nodeMap = sys->getAllNodes();
    for (NodeMap::const_iterator it = nodeMap.begin(); it != nodeMap.end(); ++it) {
        DM::Node * n = it->second;
        this->addNodeToSpatialNodeHashMap(n);
    }
}

const double &SpatialNodeHashMap::getDevider() const
{
    return this->devider;
}

System *SpatialNodeHashMap::getSystem()
{
    return this->sys;
}

SpatialNodeHashMap::~SpatialNodeHashMap()
{
    foreach(QString key, this->keys())
        delete (*this)[key];
}
}
