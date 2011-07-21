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
#include "porttuple.h"
#include <module.h>
#include <modulelink.h>
#include <port.h>


namespace vibens {
    PortTuple::PortTuple(Module * m, std::string Name, int PortType)
    {
        this->name = Name;
        this->PortType = PortType;
        int portOutType;
        int portInType;


        if(PortType == VIBe2::OUTTUPLERASTER ||PortType == VIBe2::INTUPLERASTER ) {
            portOutType = VIBe2::OUTRASTER;
            portInType = VIBe2::INRASTER;

        }
        if(PortType == VIBe2::OUTTUPLEVECTOR ||PortType == VIBe2::INTUPLEVECTOR ) {
            portOutType = VIBe2::OUTVECTOR;
            portInType = VIBe2::INVECTOR;

        }
        if(PortType == VIBe2::OUTTUPLEDOUBLEDATA ||PortType == VIBe2::INTUPLEDOUBLEDATA ) {
            portOutType = VIBe2::OUTDOUBLEDATA;
            portInType = VIBe2::INDOUBLEDATA;

        }
            inPort = new Port(m, portInType, name, true);
            outPort = new Port (m, portOutType, name, true);
    }


    Module * PortTuple::getModule() {
        return this->m;
    }

    std::vector<ModuleLink * > PortTuple::getLinks() {
        return  std::vector<ModuleLink * >();
    }


    std::string PortTuple::getLinkedDataName() {
        return "";
    }
    Port * PortTuple::getInPort() {
        return this->inPort;
    }
    Port * PortTuple::getOutPort() {
        return this->outPort;
    }
    PortTuple::~PortTuple() {
        delete this->inPort;
        delete this->outPort;
        this->inPort = 0;
        this->outPort = 0;
    }
}
