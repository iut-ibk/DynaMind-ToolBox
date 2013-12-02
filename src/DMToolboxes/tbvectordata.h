/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>, Michael Mair <michael.mair@uibk.ac.at>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2011-2012  Christian Urich and Michael Mair
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
#include <QPolygonF>
#include <math.h>
#include <map>
#include <dmlogger.h>

namespace DM {
class System;
class Node;
class Edge;
class Face;
class View;
class Component;

}

/**
  * @addtogroup ToolBoxes
  */


/**
*
* @ingroup ToolBoxes
* @brief Lots of useful functions that make live easier
* @author Christian Urich, Michael Mair
*
*/
class DM_HELPER_DLL_EXPORT TBVectorData
{
public:
    /** @brief Returns edge identified by n1 and n2*/
    static DM::Edge * getEdge(DM::System * sys, DM::View & view, DM::Node * n1, DM::Node * n2, bool OrientationMatters = true);

    /** @brief Checks if an edged already exists. Per default the Orintation is matters.
      * If an edge exists the pointer is returned. If not a  null is returned.
      **/
    static DM::Edge * getEdge(DM::System * sys, DM::View & view, DM::Edge * e, bool OrientationMatters = true);

    /** @brief Returns pointers of the face */
    static std::vector<DM::Node *> getNodeListFromFace(DM::System * sys, DM::Face * face);

//    /** @brief Calculates 2D centroid of a face.  As z value the value of the first node is returned*/
//    static DM::Node CaclulateCentroid(DM::System * sys, DM::Face * f);

    /** @brief calculate area of a planar face */
    static double CalculateArea(DM::System * sys, DM::Face * f);

    /** @brief Creates QPolygonF */
    static QPolygonF FaceAsQPolgonF(DM::System * sys, DM::Face * f);

    /** @brief Extrudes a ploygon. The new faces are added to the system and a vector with pointer to created faces is returned.
     *  Z coorindates of the extruded walls are (z + height)
     *  If the option with lid is true the last entry in the return vector points to the lid
     */
    static std::vector<DM::Face*> ExtrudeFace(DM::System * sys, const DM::View & view, const std::vector<DM::Node*> &vp,  double height, double offset = 0, bool withLid = true);

    /** @brief Calulates v' = alphas v and returns  v' as new node */
    static DM::Node RotateVector(double (&R)[3][3], const DM::Node & n1);

    /** @brief Caluates the normal vector */
    static DM::Node NormalVector(const DM::Node & n1, const DM::Node & n2);

    /** @brief Creates a the normal vectors to given nodes. The created coordinate system is
     * right handed and orthogonal. First the normal vector e3 = n1 x n2 is calulated. To create a orthogonal system
     * it just n1 to caluclate e2 and e3 (e2 = n1 x n3; e3 = e2 x e3)
     * E = e1;e2;e3
     */
    static void CorrdinateSystem(const DM::Node & n0, const DM::Node & n1,  const DM::Node & n2, double (&E)[3][3]);

    /** @brief See CorrdinateSystem(const DM::Node & n0, const DM::Node & n1,  const DM::Node & n2, double (&E)[3][3])
     */
    static void CorrdinateSystem(std::vector<DM::Node * > const &nodes, double (&E)[3][3]) ;

    /** @brief Returns the cirection cosine between two vector */
    static double DirectionCosine(const DM::Node & n1, const DM::Node & n2);

	/** @brief Returns minimal angle between two vectors */
    static double AngelBetweenVectors(const DM::Node & n1, const DM::Node & n2);

    /** @brief Creates rotation matrix based on direction cosines, where E_from (e1_from; e2_from; e3_from)
     * and E_to (e1_to; e2_to; e3_tp)  contain their normal vectors to describe the cooridnate system.
     * Direction cosines are written in alphas.
     *
     * The rotaiton matrix enables the transformation of a vector in to a new coordinate system.
     * v' = alphas v
     *
     * To transform the vector back in it's original system the transposed alpahs matrix can be used. (alphas_inverse = alphas_transpose)
     * v = alphas_t v'
     */
    static void RotationMatrix(const double (&E_from)[3][3], const double (&E_to)[3][3], double (&alphas)[3][3]);

    static double CalculateArea(std::vector<DM::Node *> const & nodes);

    /** @brief Calculate distance of two nodes A and B */
    static double calculateDistance(DM::Node *a, DM::Node *b);

    /** @brief Returns true if a point is within a Face otherwise false */
    static bool PointWithinFace(DM::Face *f, DM::Node *n);

    /** @brief Returns ture if a point is within a Face of the face vector otherwise false */
	static bool PointWithinAnyFace(std::vector<DM::Component*> fv, DM::Node *n);

    /** @brief Returns true if start and end node of a edge are point within a face otherwise false */
    static bool EdgeWithinFace(DM::Face *f, DM::Edge *e);

    /** @brief Returns true if an edge is within one Face of the face vector otherwise false */
	static bool EdgeWithinAnyFace(std::vector<DM::Component*> fv, DM::Edge *e);

    /** @brief Returns true if a point is within a Polygon othwerwise false */
    static int CalculateWindingNumber(std::vector<DM::Node*> poly, DM::Node *n);

    /** @brief Find nearest neighbours of root node within a node field */
    static std::vector<DM::Node*> findNearestNeighbours(DM::Node *root, double maxdistance, std::vector<DM::Node *> nodefield);

    /** @brief Calculate a bounding box of a node cloud [If init is set to false x,y,h,width are the minimum boundingbox] */
    static bool getBoundingBox(std::vector<DM::Node*> nodes, double &x, double &y, double &h, double &width, bool init);

    /** @brief Returns the maximum distance of a center node to all other nodes within a point field*/
    static double maxDistance(std::vector<DM::Node*> pointfield, DM::Node* centernode);

    /** @brief Creates a circle */
    static std::vector<DM::Node> CreateCircle(DM::Node * c, double radius, int segments);

     /** @brief Creates regular grid of points */
    static std::vector<DM::Node> CreateRaster(DM::System *sys, DM::Face *f, double gridSize);

    /** @brief Add nodes and create Face */
    static DM::Face * AddFaceToSystem(DM::System * sys, std::vector<DM::Node> nodes);

    /** @brief Return node with min x, min y and min z */
    static DM::Node MinCoordinates(std::vector<DM::Node*> & nodes);

    /** @brief Caclulate the extend of a View. Returns true if everything was fine */
    static bool GetViewExtend(DM::System * sys, DM::View & view, double & x_min, double & y_min, double & x_max, double & y_max);

    /** @brief Return all nodes in Nodes View */
    static std::vector<DM::Node * > GetNodesFromNodes(DM::System * sys, DM::View & view, std::vector<DM::Node *> &nodes);

    /** @brief Return all nodes in Edges View */
    static std::vector<DM::Node * > GetNodesFromEdges(DM::System * sys, DM::View & view, std::vector<DM::Node *> &nodes);

    /** @brief Return all nodes in Faces View */
    static std::vector<DM::Node * > GetNodesFromFaces(DM::System * sys, DM::View & view, std::vector<DM::Node *> &nodes);

	/** @brief Copy Face into new System, note views and attributes are not copied */
	static DM::Face * CopyFaceGeometryToNewSystem( DM::Face * f, DM::System * to_sys);

	/** @brief Simple helper to print faces in the log window */
	static void PrintFace(DM::Face * f, DM::LogLevel loglevel);


};

#endif // TBVECTORDATA_H
