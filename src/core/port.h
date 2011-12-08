/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of VIBe2
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
#ifndef PORT_H
#define PORT_H
#include "compilersettings.h"
#include <iostream>
#include <vector>
#include <iport.h>
namespace vibens {

    class Module;
    class ModuleLink;

    class VIBE_HELPER_DLL_EXPORT  Port : public IPort
    {
    private:
        std::string linkedDataName;
        Module * module;
        std::vector<ModuleLink *> links;
        int PortType;
        bool tuplePort;
        bool fullyLinked;
    public:

        Port(Module * module, int PortType,  std::string linkedDataName, bool tuplePort = false);
        virtual ~Port();
        Module * getModule();
        std::vector<ModuleLink*> getLinks();
        void addLink(ModuleLink * link);
        std::string getLinkedDataName(){return this->linkedDataName;}
        int getPortType(){return this->PortType;}
        void removeLink(ModuleLink * l);
        bool isPortTuple() {return tuplePort;}
        bool isFullyLinked() {return this->fullyLinked;}
        void setFullyLinked(bool linked) {this->fullyLinked = linked;}




    };
}

#endif // PORT_H
