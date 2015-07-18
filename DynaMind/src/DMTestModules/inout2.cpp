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

#include "inout2.h"
#include <dmcomponent.h>
#include <dmsystem.h>
#include <dmnode.h>


DM_DECLARE_NODE_NAME( InOut2,Modules )
InOut2::InOut2() {


    Logger(Debug) << "Create InOut";
    std::vector<DM::View> views;
    inlets = DM::View("Inlets", DM::NODE, DM::READ);
    inlets.addAttribute("A", DM::Attribute::NOTYPE, DM::READ);
    inlets.addAttribute("B", DM::Attribute::NOTYPE, DM::READ);
    inlets.addAttribute("C", DM::Attribute::NOTYPE, DM::READ);
    inlets.addAttribute("D", DM::Attribute::NOTYPE, DM::READ);
    inlets.addAttribute("F", DM::Attribute::NOTYPE, DM::WRITE);
    conduits = DM::View("Conduits", DM::EDGE,  DM::READ);
    conduits.addAttribute("F", DM::Attribute::NOTYPE, DM::WRITE);

    views.push_back(inlets);
    views.push_back(conduits);

    this->addData("Inport", views);
    a = 0;
    this->addParameter("a",DM::DOUBLE, &a);
    a = 5;

}



void InOut2::run() {


    int test = a;
    Logger(Debug) << "Run InOut";
    //Logger(Debug) << this->getParameterAsString("a");
    sys_in = this->getData("Inport");
	/*
    std::map<std::string, DM::Node*> all_nodes = sys_in->getAllNodes();

    for (std::map<std::string, DM::Node*>::const_iterator it = all_nodes.begin(); it != all_nodes.end(); ++it)
    {
        DM::Node * n = it->second;
    }
	*/
    DM::Node * n1 = sys_in->addNode(0,0,2, inlets);
    DM::Node * n2 = sys_in->addNode(0,0,3, inlets);

    double b = 0;

    /*std::map<std::string, DM::System*> subs = sys_in->getAllSubSystems();

    for (std::map<std::string, DM::System*>::const_iterator it = subs.begin(); it != subs.end(); ++it)
    {
        DM::System * s = it->second;*/
	foreach(DM::System* s, sys_in->getAllSubSystems())
	{
        Logger(Debug) << s->getUUID() << " " << s->getAllNodes().size() << " " << s->getAllEdges().size();
    }


    sys_in->addEdge(n1, n2, conduits);
	/*
    std::vector<std::string> nstreets = sys_in->getUUIDsOfComponentsInView(streets);
    foreach (std::string str, nstreets) {
        Edge * e = sys_in->getEdge(str);
        e->getAllAttributes();
    }
	*/

}

InOut2::~InOut2() {
    Logger(Debug) << "Destructor InOut";
}
