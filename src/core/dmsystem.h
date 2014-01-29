/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @author  Michael Mair <abroxos@gmail.com>
 * @author  Markus Sengthaler <m.sengthaler@gmail.com>
 * @version 1.0
 * @section LICENSE
 * This file is part of DynaMind
 *
 * Copyright (C) 2011  Christian Urich, Michael Mair, Markus Sengthaler

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
#include <dmcomponent.h>

#ifdef SWIG
#define DM_HELPER_DLL_EXPORT
#endif

namespace DM {

class Node;
class Edge;
class Face;
class RasterData;
class Module;

typedef std::pair<std::string, Component*> ComponentPair;
typedef std::pair<std::string, Attribute*> AttributePair;
typedef std::pair<std::string, RasterData*> RasterDataPair;
typedef std::pair<std::string, Edge*> EdgePair;
typedef std::pair<std::string, Node*> NodePair;
typedef std::pair<std::string, Face*> FacePair;

class DerivedSystem;

/** @class DM::System
  * @ingroup DynaMind-Core
  * @brief The system class provides a description for complex objects.
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
    friend class DerivedSystem;
public:
    bool removeChild(Component* c);

    /*@deprecated*/
    std::vector<Component*> getAllChilds();
    std::vector<Component*> getChilds();

    /** @brief Copies a System  */
    System(const System& s); // needs to be redone

    /** @brief creates a new System */
    System();

    /** @brief Destructor
    * The destructor also deletes all successor states */
    ~System();

    /** @brief return Type */
    Components getType() const;

    /** @brief Adds an existing component to the system. The ownership of the component goes to the system*/
    Component * addComponent(Component* c, const DM::View & view = DM::View());

    /** @brief Adds an existing node to the system. The ownership of the node goes to the system.
    *  If the node already exists 0 it returns 0, if not the pointer to the node (same as the input)
    */
    Node * addNode(Node* node);

    /** @brief Adds a new node to the system and returns a pointer to the node.*/
    Node * addNode(double x, double y, double z, const DM::View & view = DM::View());

    /** @brief Copies xyz in a new Node, attaches it to the system, returning a pointer*/
    Node * addNode(const Node &n, const DM::View & view = DM::View());

    /** @brief Adds a new Edge to the system, the system class takes ownership of the edge */
    Edge* addEdge(Edge* edge, const DM::View & view = DM::View());

    /** @brief Creates a new Edge, based on the UUID of the start and end node */
    Edge* addEdge(Node * start, Node * end, const DM::View & view = DM::View());

    /** @brief Adds a new Face to the system, the system class takes ownership of the face  */
    Face * addFace(Face * f);

    /** @brief Creates a new Face, based on the UUID of the nodes stored in the vector */
    Face * addFace(std::vector<Node*> nodes,  const DM::View & view = DM::View());

    /** @brief Returns a pointer to the edge. Returns 0 if Edge doesn't exist */
    virtual Edge* getEdge(Node* start, Node* end);

    /** @brief Returns a map of nodes stored in the system */
    virtual std::vector<Component*> getAllComponents();

    /** @brief Returns a map of nodes stored in the system */
    virtual std::vector<Node*> getAllNodes();

    /** @brief Returns a map of edges stored in the system */
    virtual std::vector<Edge*> getAllEdges();

    /** @brief Returns a map of faces stored in the system */
    virtual std::vector<Face*> getAllFaces();

    /** @brief Returns a map of subsystems stored in the system */
    virtual std::vector<System*> getAllSubSystems();

    /** @brief Returns a map of rasterdata stored in the system */
    virtual std::vector<RasterData*> getAllRasterData();

    /** @brief Returns the predecessor of the system */
    virtual System* getPredecessor() const;

    /** @brief Returns the sucessor of the system */
    std::vector<System*> getSucessors() const;

    /** @brief adds a new subsystem, the system class takes ownership of the subsystem*/
    System * addSubSystem(System *newsystem, const DM::View & view = DM::View());

    /** @brief Creates a new Successor state
    * @todo add a more detailed description here
    */
    System* createSuccessor();

    /** @brief Creates a clone of the System. UUID and Attributes stay the same as its origin */
    Component* clone();

    /** @brief add a component to a view */
    bool addComponentToView(Component * comp, const DM::View & view);

    /** @brief remove a component from a view */
    bool removeComponentFromView(Component * comp, const DM::View & view);
    bool removeComponentFromView(Component * comp, const std::string& viewName);

    /** @brief retrun all components related to a view */
    virtual std::vector<Component*> getAllComponentsInView(const DM::View &view);

    /** @brief Adds raster data to the system. The owner ship of the raster data is taken by the system */
    RasterData * addRasterData(RasterData * r,  const DM::View & view = DM::View());

	virtual Component* _getChild(QUuid quuid);

	virtual Component* _getChildReadOnly(QUuid quuid);

    virtual Component* getSuccessingComponent(const Component* formerComponent);

    void updateViews(const std::vector<View>& views);

    //void loadFromHost();

    /** @brief exports the component to the db, it can be deleted safly afterwards */
    virtual void _moveToDb();

    /** @brief imports all components according to the currently applied views (update view) */
    void _importViewElementsFromDB();
protected:
    const Edge* getEdgeReadOnly(Node* start, Node* end);
private:
    void SQLInsert();
    void SQLUpdateStates();
    bool addChild(Component *newcomponent);
    /** @brief return table name */
    QString getTableName();

    //DM::Module* lastModule;
    std::set<Node* >			nodes;
    std::set<Edge* >			edges;
    std::set<Face* >			faces;
    std::set<RasterData *>	rasterdata;
    std::set<System*>		subsystems;
    std::set<Component* >	components;

    std::map<QUuid, Component*>	quuidMap;

    std::vector<DM::System*> sucessors;

    //std::map<std::string, std::vector<Component*> > views;
    class ViewCache
    {
    public:
        struct Equation
        {
            Equation()
            {
                varName = "";
                axis = NONE;
                op = EQUAL;
                val = 0.0;
            }

            std::string varName;
            enum CoordinateAxis{NONE,X,Y,Z} axis;
            enum Operator{EQUAL, LEQUAL, HEQUAL, LOWER, HIGHER} op;
            double val;

            bool eval(Component* c) const;
        }eq;

        void apply(const View& view);
        bool add(Component* c);
        bool remove(Component* c);
        bool legal(Component* c);

        std::set<Component*> filteredElements;
        System* sys;
        std::set<QUuid>	rawElements;
    //private:
        View view;
    };

    std::map<std::string, ViewCache > viewCaches;
};

typedef std::map<std::string, DM::System*> SystemMap;
}

#endif // SYSTEM_H
