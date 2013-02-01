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
/*
#include "dmporttuple.h"
#include <dmmodule.h>
#include <dmmodulelink.h>
#include <dmport.h>


namespace DM {
	
PortTuple::PortTuple(Module * m, std::string Name, int PortType, bool Inport)
{
    this->name = Name;
    this->PortType = PortType;
    inPort = new Port(m, DM::INSYSTEM, name, true, Inport);
    outPort = new Port (m, DM::OUTSYSTEM, name, true, Inport);
}


Module * PortTuple::getModule() {
    return this->m;
}

std::vector<ModuleLink * > PortTuple::getLinks() {
    return  std::vector<ModuleLink * >();
}


std::string PortTuple::getLinkedDataName() const {
    return "";
}
Port * PortTuple::getInPort() const {
    return this->inPort;
}
Port * PortTuple::getOutPort() const {
    return this->outPort;
}
PortTuple::~PortTuple() {
    delete this->inPort;
    delete this->outPort;
    this->inPort = 0;
    this->outPort = 0;
}

}
*/