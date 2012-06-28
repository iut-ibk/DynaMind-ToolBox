/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2011  Christian Urich
 
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

#include "guisimulation.h"
#include <modelnode.h>
#include <groupnode.h>

GUISimulation::GUISimulation() : Simulation()
{

}

void GUISimulation::changeGroupName(GroupNode * g) {
    emit GroupNameChanged(g);
}

GroupNode * GUISimulation::getGroupNode(DM::Group * g) {

    foreach (GroupNode * gn, this->groupNodes) {
        if (gn->getDMModel() == g)
            return gn;
    }

    return 0;
}

void GUISimulation::GUIaddModule( DM::Module * m, QPointF pos)
{
    //Create Visual Representation of the Module
    if (!m->isGroup()) {
        ModelNode * node = new ModelNode(m, this);
        this->modelNodes.append(node);
        node->setPos(pos);
        emit addedModule(node);
    }
    if (m->isGroup()) {
        GroupNode * node = new GroupNode(m, this);
        this->groupNodes.append(node);
        node->setPos(pos);
        emit addedGroup(node);
    }

    //this->updateSimulation();

}

void GUISimulation::registerRootNode() {
    this->GUIaddModule(this->getRootGroup(), QPointF(0,0));
}

void GUISimulation::GUIaddModule(QString name, QPointF pos, DM::Module *group)
{
    //Create Module in DynaMind

    DM::Module * m = this->addModule(name.toStdString());
    if (!m) {
        DM::Logger(DM::Error) << "Couldn't Create Module " << name.toStdString();
        return;
    }
    m->setGroup((DM::Group*)group);
    this->GUIaddModule(m, pos);

}



void GUISimulation::updateSimulation()
{    
    this->startSimulation(true);
}

void GUISimulation::clearSimulation() {

    GroupNode * rg = this->getGroupNode((DM::Group*)this->getRootGroup());
    foreach (GroupNode * g, this->groupNodes) {
        if (g != rg)
        delete g;
    }
    this->groupNodes.clear();
    this->groupNodes.push_back(rg);

    foreach (ModelNode * m, this->modelNodes) {
        delete m;
    }
    this->modelNodes.clear();

    this->setSimulationStatus(DM::SIM_OK);

}

void GUISimulation::showHelp(string classname) {
    emit showHelpForModule(classname);
}
