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
#include "dmport.h"
#include <dmmodulelink.h>
#include <dmmodule.h>
#include <boost/foreach.hpp>
using namespace boost;
namespace DM {
    Port::Port(Module * module, int PortType, std::string linkedDataName, bool tuplePort)
    {
        this->module = module;
        this->PortType = PortType;
        this->linkedDataName = linkedDataName;
        this->tuplePort = tuplePort;
        this->fullyLinked = false;
    }
    Module * Port::getModule() {
        return this->module;
    }
    std::vector<ModuleLink*> Port::getLinks() {
        return this->links;
    }
    void Port::addLink(ModuleLink * link) {
        this->links.push_back(link);
    }
    Port::~Port() {
       DM::Logger(DM::Debug) << "delete Port " << this->linkedDataName;
        while (this->links.size() > 0)
            delete *(this->links.begin());
    }

    void Port::removeLink(ModuleLink * l) {
        Logger(Debug) << this->links.size();
        for (std::vector<ModuleLink* >::iterator it = this->links.begin(); it!=this->links.end();) {
            if (*it == l) {
                it = this->links.erase(it);
            } else {
                ++it;
            }

        }
         Logger(Debug) << "Remove Link" << this->links.size();
    }

}
