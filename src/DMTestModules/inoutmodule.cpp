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

#include "inoutmodule.h"
#include <dmcomponent.h>
#include <dmsystem.h>
#include <dmnode.h>

DM_DECLARE_NODE_NAME( InOut,Modules )
InOut::InOut() {
    Logger(Debug) << "Create InOut";
    std::vector<DM::View> views;
    inlets = DM::View("Inlets",DM::NODE, DM::MODIFY);
    inlets.getAttribute("A");
    inlets.getAttribute("B");
    inlets.addAttribute("C");
    conduits = DM::View("Conduits", DM::EDGE, DM::MODIFY);
    conduits.addAttribute("D");
    conduits.addAttribute("F");

    views.push_back(inlets);
    views.push_back(conduits);

    this->addData("Inport", views);
    a = 0;
    this->addParameter("a",DM::DOUBLE, &a);
    a = 5;

}

void InOut::run() {
    int test = a;
    Logger(Debug) << "Run InOut";
    //Logger(Debug) << this->getParameterAsString("a"); TODO: maybe replace by analogon
    sys_in = this->getData("Inport");

    std::map<std::string, DM::Node*> all_nodes = sys_in->getAllNodes();

    for (std::map<std::string, DM::Node*>::const_iterator it = all_nodes.begin(); it != all_nodes.end(); ++it)
    {
        DM::Node * n = it->second;
        //Logger(Debug) << n->getName() << n->getX() << n->getY() << n->getZ();
    }

    DM::Node * n1 = sys_in->addNode(0,0,2, inlets);
    DM::Node * n2 = sys_in->addNode(0,0,3, inlets);


    sys_in->addEdge(n1, n2, conduits);

    Logger(Debug) <<  "Number of stuff "<< sys_in->getAllEdges().size();
}

InOut::~InOut() {
    Logger(Debug) << "Destructor InOut";
}
