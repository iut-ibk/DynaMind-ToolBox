/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @author  Michael Mair <abroxos@gmail.com>
 * @version 1.0
 * @section LICENSE
 * This file is part of DynaMite
 *
 * Copyright (C) 2011  Christian Urich, Michael Mair

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

#ifndef DMSYSTEM_H
#define DMSYSTEM_H
#include <dmcompilersettings.h>
#include <map>
#include <vector>
#include <dmview.h>



#ifdef SWIG
#define DM_HELPER_DLL_EXPORT
#endif

namespace DM {

enum Components {
    COMPONENT,
    NODE,
    EDGE,
    FACE,
    SUBSYSTEM,
    RASTERDATA
};

class Component;
class Node;
class Edge;
class Face;
class RasterData;

/** @ingroup DynaMind-Core
      * @brief The system class provides a description for comlpex objects.
      *
      * Systems can be described with nodes, edges, faces, rasterdata. Systems can contain sub systems.
      * Systems are used to describe urban environment - water infrastructure, streets, houses ...
      *
      * The System class is derived from the Component class. Therefore every system has a UUID and can hold Attributes.
      *
      * To use the System class in a dynamic environment it is possible to create a successor state. Successor states hold a new list of pointer to
      * the objects stored in the system. If a Object is added, removed or changed only the successor system is altered.
      */
class  DM_HELPER_DLL_EXPORT System : public Component
{
private:
    std::vector<System*> predecessors;
    std::map<std::string, Node* > nodes;
    std::map<std::string, Edge* > edges;
    std::map<std::string, Face* > faces;
    std::map<std::string, RasterData *> rasterdata;
    std::map<std::string, System*> subsystems;
    std::map<std::string, Component* > components;
    std::map<std::string, View> viewdefinitions;
    std::map<std::string, std::map<std::string, Component*> > views;
    DM::System * sucessor;


    //Get Edge Based on otherwise takes ages
    std::map<std::pair<std::string ,std::string>,DM::Edge*> EdgeNodeMap;

    void updateViews (Component * c);

public:

    /** @brief creates a new System */
    System();    
    /** @brief Copies a System  */
    System(const System& s);
    ~System();
    /** @brief Adds an existing component to the system. The ownership of the component goes to the system*/
    Component * addComponent(Component* c, const DM::View & view = DM::View());
    /** @brief Adds an existing node to the system. The ownership of the node goes to the system*/
    Node * addNode(Node* node);
    /** @brief Adds a new node to the system and returns a pointer to the node.*/
    Node * addNode(double x, double y, double z, const DM::View & view = DM::View());
    /** @brief Adds a new node to the system and returns a pointer to the node. */
    Node * addNode(Node node,  const DM::View & view = DM::View());
    /** @brief Adds a new Edge to the system, the system class takes ownership of the edge */
    Edge* addEdge(Edge* edge);
    /** @brief Creates a new Edge, based on the UUID of the start and end node */
    Edge* addEdge(Node * start, Node * end, const DM::View & view = DM::View());
    /** @brief Adds a new Face to the system, the system class takes ownership of the face  */
    Face * addFace(Face * f);
    /** @brief Creates a new Face, based on the UUID of the nodes stored in the vector */
    Face * addFace(std::vector<Node*> nodes,  const DM::View & view = DM::View());
    /** @brief Returns a pointer to the component. Returns 0 if Component doesn't exist*/
    Component* getComponent(std::string uuid);
    /** @brief Returns a pointer to the node. Returns 0 if Node doesn't exist*/
    Node* getNode(std::string uuid);
    /** @brief Returns a pointer to the edge. Returns 0 if Edge doesn't exist*/
    Edge* getEdge(std::string uuid);
    /** @brief Returns a pointer to the edge. Returns 0 if Edge doesn't exist*/
    Edge* getEdge(const std::string &startnodeuuid, const std::string &endnodeuuid);
    /** @brief Returns a pointer to the face. Returns 0 if Face doesn't exist*/
    Face * getFace(std::string uuid);
    /** @brief Removes an Edge. Returns false if the edge doesn't exist */
    bool removeEdge(std::string uuid);
    /** @brief Removes a Node. Returns false if the node doesn't exist */
    bool removeNode(std::string uuid);
    /** @brief Removes a Component. Returns false if the component doesn't exist */
    bool removeComponent(std::string uuid);
    /** @brief Removes a Face. Returns false if the face doesn't exist */
    bool removeFace(std::string uuid);
    /** @brief Returns a map of nodes stored in the system */
    std::map<std::string, Node*> getAllNodes();
    /** @brief Returns a map of edges stored in the system */
    std::map<std::string, Edge*> getAllEdges();
    /** @brief Returns a map of faces stored in the system */
    std::map<std::string, Face*> getAllFaces();
    /** @brief Returns a map of subsystems stored in the system */
    std::map<std::string, System*> getAllSubSystems();
    /** @brief Returns a map of rasterdata stored in the system */
    std::map<std::string, RasterData*> getAllRasterData();
    /** @brief Returns the predecessor of the system */
    std::vector<System*> getPredecessorStates();
    /** @brief adds a new subsystem, the system class takes ownership of the subsystem*/
    bool addSubSystem(System *newsystem, const DM::View & view = DM::View());
    /** @brief Removes a Subsystem. Returns false if the subsystem doesn't exist */
    bool removeSubSystem(std::string uuid);
    /** @brief Returns Subsystem. Returns 0 if Subsystem doesn't exist */
    System* getSubSystem(std::string uuid);
    /** @brief Creates a new Successor state
       *
       * @todo add a more detailed description here
       */
    System* createSuccessor();
    /** @brief Adds a new view to the system. At the moment always returns true */
    bool addView(DM::View view);
    /** @brief return a vector of the names of the views avalible in the system */
    std::vector<std::string> getNamesOfViews();
    /** @brief return a vector of views avalible in the system */
    const std::vector<DM::View> getViews();
    /** @brief Retruns View */
    View getViewDefinition(std::string name);
    /** @brief Creates a clone of the System. UUID and Attributes stay the same as its origin */
    Component* clone();
    /** @brief add a component to a view */
    bool addComponentToView(Component * comp, const DM::View & view);
    /** @brief remove a component from a view */
    bool removeComponentFromView(Component * comp, const DM::View & view);
    /** @brief retrun all components related to a view */
    std::map<std::string, Component*> getAllComponentsInView(const View &view);
    /** @brief Returns a vector of all uuids stored in a view */
    std::vector<std::string> getUUIDsOfComponentsInView(DM::View  view);
    /** @brief Adds raster data to the system. The owner ship of the raster data is taken by the system */
    RasterData * addRasterData(RasterData * r,  const DM::View & view = DM::View());
    /** @brief add Predecessor **/
    void addPredecessors(DM::System * s);
};

typedef std::map<std::string, DM::System*> SystemMap;
}
#endif // SYSTEM_H
