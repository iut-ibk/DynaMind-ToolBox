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

#ifndef SYSTEM_H
#define SYSTEM_H

#include <map>
#include <vector>
#include <DMview.h>
namespace DM {
    enum Components {
        NODE,
        EDGE,
        SUBSYSTEM
    };

    class Component;
    class Node;
    class Edge;

    class System : public Component
    {
    private:
        std::vector<System*> predecessors;
        std::map<std::string, Node* > nodes;
        std::map<std::string, Edge* > edges;
        std::map<std::string, System*> subsystems;
        std::map<std::string, View> viewdefinitions;
        std::map<std::string, std::map<std::string, Component*> > views;

        void updateViews (Component * c);

    public:
        System(std::string name, std::string view);
        System(const System& s);
        ~System();

        Node * addNode(Node* node);
        Node * addNode(double x, double y, double z, std::string view = "");
        Edge* addEdge(Edge* edge);
        Edge* addEdge(Node * start, Node * end, std::string view = "");

        Node* getNode(std::string name);
        Edge* getEdge(std::string name);
        bool removeEdge(std::string name);
        bool removeNode(std::string name);
        std::map<std::string, Node*> getAllNodes();
        std::map<std::string, Edge*> getAllEdges();
        std::vector<System*> getPredecessorStates();
        bool addSubSystem(System *newsystem);
        bool removeSubSystem(std::string name);
        System* getSubSystem(std::string name);
        std::map<std::string, System*> getAllSubSystems();
        System* createSuccessor();
        bool addView(DM::View view);
        std::vector<std::string> getNamesOfViews();
        Component* clone();

    };
}
#endif // SYSTEM_H
