/**
 * @file
 * @author  Michael Mair <michael.mair@uibk.ac.at>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2012  Michael Mair

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

#ifndef Watersupplydef_H
#define Watersupplydef_H

#include <dm.h>
#include <string.h>

namespace DM
{
    namespace WS
    {
        #define EPANETMODEL_ATTR \
        A(Inpfilepath)

        #define EPANETMODEL_LINK

        #define JUNCTION_LINK

        #define JUNCTION_ATTR \
        A(Elevation)

        #define DEMAND_ATTR \
        A(Demand)

        #define DEMAND_LINK

        #define PIPE_LINK

        #define PIPE_ATTR\
        A(Diameter) \
        A(Length)

        #define TABLE \
        X(JUNCTION,DM::NODE, JUNCTION_ATTR, JUNCTION_LINK) \
        X(PIPE,DM::EDGE,PIPE_ATTR, PIPE_LINK) \
        X(EPANETMODEL,DM::COMPONENT,EPANETMODEL_ATTR, EPANETMODEL_LINK) \
        X(DEMAND,DM::NODE,DEMAND_ATTR,DEMAND_LINK)

        #include<viewdefhelper.h> //This would be the header if code completion would work correctly in qtcreator
    }
}

#endif // Watersupplydef_H
