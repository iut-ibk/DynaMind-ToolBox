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

#ifndef IPORT_H
#define IPORT_H
#include "compilersettings.h"
#include <iostream>
#include <vector>

namespace vibens {

    class Module;
    class ModuleLink;

    class VIBE_HELPER_DLL_EXPORT  IPort {
    public:
        virtual ~IPort() {}
        virtual int getPortType() = 0;
        virtual Module * getModule() = 0;
        virtual std::vector<ModuleLink * > getLinks() = 0;
        virtual std::string getLinkedDataName() = 0;
    };
}
#endif // IPORT_H
