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

#ifndef DMGEOMETRY_H
#define DMGEOMETRY_H

#include <dm.h>

#include <QString>
#include <QHash>


typedef QHash<QString, std::vector<DM::Node* > *> NodeHashMap;

namespace DM {

/** @ingroup ToolBoxes
  * @brief Lots of useful functions that make live easier
  * @author Christian Urich
 */
class DM_HELPER_DLL_EXPORT SpatialNodeHashMap : public NodeHashMap{
private:
    double devider;
    DM::System * sys;
public:
    /** @brief addNode to spatial node has map. */
    void addNodeToSpatialNodeHashMap(DM::Node * n);
    SpatialNodeHashMap(DM::System * sys, double devider, bool init = true, const DM::View & nodeView = DM::View());
    const double & getDevider() const;
    DM::System * getSystem();
    ~SpatialNodeHashMap();


public:
    /** @brief Creates a spatial hash from a node
     *
     * the key is concatenated out of the int value x/devider '|' and y/devider
     * e.g. x=240.00; y=200.00; devider=20,00 -> 12|10
     */
    QString spatialHashNode(const double &x, const double &y);

    /** @brief Returns the pointer to the node located at x,y (including a tolerance) or 0 if it couldn't find a node.
      * It requires a NodeHasMap that can be created with the initNodeHasMap method
      */
    DM::Node * findNode(const double & x, const double & y, const double & tol);

    /** @brief Adds a node to the system. Before a new node is created it checks if already a node exists.
     * Returns either the pointer to the new node or to an existing one.
     *
     */
    DM::Node * addNode(double x, double y, double z, double tol, DM::View v = DM::View());

    /** @brief Adds nodes from view to the search field
     *
     */
    void addNodesFromView(const DM::View & view);
};
}

#endif // DMGEOMETRY_H