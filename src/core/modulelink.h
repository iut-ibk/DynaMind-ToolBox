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
#ifndef LINK_H
#define LINK_H
#include "compilersettings.h"
#include <iostream>

namespace DM {
    class Port;
    class DM_HELPER_DLL_EXPORT  ModuleLink
    {
    private:
        Port * InPort;
        Port * OutPort;
        bool backLink;
        ModuleLink * getPortFromTuplePort(ModuleLink * origin);

    public:
        ModuleLink(Port * InPort, Port * OutPort,bool isBackPort = false);
        Port * getInPort();
        Port * getOutPort();
        bool isBackLink();
        bool isBackLinkFromOrigin;
        bool isBackLinkInChain();
        void setBackLink(bool b){this->backLink = b;}
        std::string getDataNameFromOutPort();
        std::string getUuidFromOutPort();
        void setInPort(Port *);
        void setOutPort(Port *);
        ~ModuleLink();
    };
}
#endif // LINK_H
