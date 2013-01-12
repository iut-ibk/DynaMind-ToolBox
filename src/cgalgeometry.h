/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2011-2012   Christian Urich

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

#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "dmcompilersettings.h"
#include "dm.h"

namespace DM {


    class DM_HELPER_DLL_EXPORT CGALGeometry
    {
    public:

        static DM::System ShapeFinder(DM::System * sys, DM::View & id, DM::View & return_id, bool withSnap_Rounding = false,  float Tolerance=0.01, bool RemoveLines=true);

        /** @brief Calculates minimal bounding box. Returns alpha in degree, the 4 nodes of the bounding box and the size (l and w) */
        static double CalculateMinBoundingBox(std::vector<DM::Node*> nodes, std::vector<DM::Node> &boundingBox, std::vector<double> & size);
        
        static std::vector<DM::Node> OffsetPolygon(std::vector<DM::Node*> points, double offset);

        /** @brief Returns node list that contains the triangulation of the face f.
         * Every trinagle is defined by 3 nodes.
         */
        static std::vector<DM::Node> FaceTriangulation(DM::System * sys, DM::Face * f);

        /** @brief Regular Triangulation
         */
        static std::vector<DM::Node> RegularFaceTriangulation(DM::System * sys, DM::Face * f, std::vector<int> & ids, double meshsize);

        /** @brief Intersect Faces */
        static std::vector<DM::Node> IntersectFace(DM::System * sys, DM::Face * f1, DM::Face * f2);

        static bool DoFacesInterect(std::vector<DM::Node*> nodes1, std::vector<DM::Node*> nodes2);

        /** @brief Rotate Nodes */
        static std::vector<DM::Node> RotateNodes(std::vector<DM::Node> nodes, double alpha);

        /** @brief Check Orientation if CLOCKWISE return true */
        static bool CheckOrientation(std::vector<DM::Node*> nodes);

    };
}
#endif // GEOMETRY_H
