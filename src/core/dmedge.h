/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @author  Michael Mair <abroxos@gmail.com>
 * @version 1.0
 * @section LICENSE
 * This file is part of DynaMite
 *
 * Copyright (C) 2011  Christian Urich, Michael Mair

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

#ifndef DMEDGE_H
#define DMEDGE_H
#include <dmcompilersettings.h>

namespace DM {

    class Component;

    class DM_HELPER_DLL_EXPORT Edge : public Component
    {
    private:
        std::string start;
        std::string end;

    public:
        Edge(std::string startpoint, std::string endpoint);
        Edge(const Edge& e);
        std::string getStartpointName();
        std::string getEndpointName();
        void setStartpointName(std::string name);
        void setEndpointName(std::string name);
        Component* clone();
    };
}
#endif // Edge_H
