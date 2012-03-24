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
#include <dmmodulelink.h>
#include <dmport.h>
#include <dmmodule.h>
#include <dmgroup.h>
#include <dmporttuple.h>
namespace DM {
ModuleLink::ModuleLink(Port * InPort, Port * OutPort, bool isBackPort)
{
    this->InPort = InPort;
    this->InPort->addLink(this);
    this->OutPort = OutPort;
    this->OutPort->addLink(this);
    this->backLink = isBackPort;
    isBackLinkFromOrigin = false;

}
void ModuleLink::setInPort(Port * p) {
    this->InPort = p;
    this->InPort->addLink(this);
}
void ModuleLink::setOutPort(Port * p) {
    this->OutPort = p;
    this->OutPort->addLink(this);
}
Port * ModuleLink::getInPort() {
    return this->InPort;
}
Port * ModuleLink::getOutPort() {
    return this->OutPort;
}
std::string ModuleLink::getUuidFromOutPort() {
    if (this->OutPort->isPortTuple()) {
        if (getPortFromTuplePort(this, this->OutPort->isInPortTuple()) != 0 )
            return getPortFromTuplePort(this, this->OutPort->isInPortTuple())->getOutPort()->getModule()->getUuid();
    }

    return this->OutPort->getModule()->getUuid();
}
std::string ModuleLink::getDataNameFromOutPort() {
    if (this->OutPort->isPortTuple()) {
        if (getPortFromTuplePort(this, this->OutPort->isInPortTuple()) != 0 )
            return getPortFromTuplePort(this, this->OutPort->isInPortTuple())->getOutPort()->getLinkedDataName();
    }
    return this->OutPort->getLinkedDataName();
}
ModuleLink *  ModuleLink::getPortFromTuplePort(ModuleLink * origin, bool fromInportTuple) {

    Group * g = (Group*) this->OutPort->getModule();
    PortTuple * pt = g->getInPortTuple(this->OutPort->getLinkedDataName());
    if (pt == 0 || !fromInportTuple) {
        pt = g->getOutPortTuple(this->OutPort->getLinkedDataName());
    }

    Port * p =  pt->getInPort();

    int LinkId = -1;
    int BackId = -1;
    int counter = 0;
    foreach (ModuleLink * l, p->getLinks()) {
        if (!l->isBackLink()||p->getLinks().size() == 1) {
            LinkId = counter;
        } else {
            BackId = counter;
        }
        counter++;
    }
    if (LinkId < 0) {
        return 0;
    }
    ModuleLink *l = p->getLinks()[LinkId];

    if (g->getInternalCounter() > 1 && BackId != -1){
        l = p->getLinks()[BackId];
        Logger(Debug)<< "Internal Counter" << g->getInternalCounter();
        Logger(Debug) << "BackLink for TuplePort" << this->OutPort->getLinkedDataName();
        origin->isBackLinkFromOrigin = true;

        Logger(Debug) << "Set Back link From Origin" << origin->getInPort()->getLinkedDataName();

    }
    if (l == 0)
        return 0;
    if (l->getOutPort()->isPortTuple()) {
        l = l->getPortFromTuplePort(origin,l->getOutPort()->isInPortTuple());
    }
    if (l == 0)
        return 0;

    if (origin->isBackLinkFromOrigin)
        l->isBackLinkFromOrigin = true;
    return l;
}
ModuleLink::~ModuleLink() {
    Logger(Debug) << "Remove Link";
    if (InPort != 0)
        this->InPort->removeLink(this);
    if (OutPort != 0)
        this->OutPort->removeLink(this);

}


bool ModuleLink::isBackLink() {

    return this->backLink;
}
bool ModuleLink::isBackLinkInChain() {
    if (this->OutPort->isPortTuple()) {

        Group * g = (Group*) this->OutPort->getModule();
        PortTuple * pt = g->getInPortTuple(this->OutPort->getLinkedDataName());
        if (pt == 0) {
            pt = g->getOutPortTuple(this->OutPort->getLinkedDataName());
        }
        Port * p =  pt->getInPort();
        int LinkId = -1;
        int BackId = -1;
        int counter = 0;
        foreach (ModuleLink * l, p->getLinks()) {

            if (!l->isBackLink()) {//||p->getLinks().size() == 1) {
                LinkId = counter;
            } else {
                BackId = counter;
            }
            counter++;

        }
        ModuleLink *l = p->getLinks()[LinkId];
        if (g->getInternalCounter() > 1 && BackId != -1 && counter > 1){
            l = p->getLinks()[BackId];
            return true;
        }
        else if (BackId != -1 && counter == 1){
            l = p->getLinks()[BackId];
            return true;

        }


    }

    return this->isBackLink();
}

}
