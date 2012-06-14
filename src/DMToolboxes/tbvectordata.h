/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2011-2012  Christian Urich
 
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

#ifndef TBVECTORDATA_H
#define TBVECTORDATA_H
#include <dmcompilersettings.h>
#include <vector>
namespace DM {
    class System;
    class Node;
    class Edge;
    class Face;
    class View;
}

/**
  * @addtogroup ToolBoxes
  */


/**
*
* @ingroup ToolBoxes
* @brief Lots of useful functions that make live easier
* @author Christian Urich
*
*/
class DM_HELPER_DLL_EXPORT TBVectorData
{
public:
    /**
      Checks if an edged already exists. Per default the Orintation is matters.
      If an edge exists the pointer s returned. If not a  null is returned.
      **/
    static DM::Node * getNode2D(DM::System * sys, DM::View  &view, DM::Node  n, double err = 0);

     /** @brief Checks if the a node already exists in the system. If not a new node is created, otherwise a new node is created.*/
    static DM::Node * addNodeToSystem2D(DM::System *sys,  DM::View & view, DM::Node   n1, double err=0,  bool CreateNewNode = true);
    /** @brief returns pointers of the face */
    static std::vector<DM::Node *> getNodeListFromFace(DM::System * sys, DM::Face * face);

    static void splitEdge(DM::System * sys, DM::Edge * e, DM::Node * n, DM::View & view);

    /** @brief calculate centroid of a face */
    static DM::Node caclulateCentroid(DM::System * sys, DM::Face * f);
    /** @brief calculate area of a face */
    static double calculateArea(DM::System * sys, DM::Face * f);


};

#endif // TBVECTORDATA_H
