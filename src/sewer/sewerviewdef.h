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

#ifndef Sewerdef_H
#define Sewerdef_H

#include <dm.h>
#include <string.h>

namespace DM
{
    namespace S
    {
        #define WEIR_LINK

        #define WEIR_ATTR

        #define CATCHMENT_LINK

        #define CATCHMENT_ATTR \
		A(Area,DM::Attribute::DOUBLE) \
		A(Impervious,DM::Attribute::DOUBLE) \
		A(Raingage,DM::Attribute::DOUBLE)

        #define JUNCTION_LINK

        #define JUNCTION_ATTR \
		A(DWF,DM::Attribute::DOUBLE)

        #define CONDUIT_LINK

        #define CONDUIT_ATTR \
		A(Diameter,DM::Attribute::DOUBLE) \
		A(Length,DM::Attribute::DOUBLE)

        #define OUTFALL_LINK

        #define OUTFALL_ATTR

        #define INLET_LINK

        #define INLET_ATTR \
		A(CATCHMENT,DM::Attribute::DOUBLE)

        #define TABLE \
        X(JUNCTION,DM::NODE, JUNCTION_ATTR, JUNCTION_LINK) \
        X(CONDUIT,DM::EDGE,CONDUIT_ATTR, CONDUIT_LINK) \
        X(OUTFALL, DM::NODE, OUTFALL_ATTR, OUTFALL_LINK) \
        X(INLET, DM::NODE, INLET_ATTR, INLET_LINK) \
        X(CATCHMENT, DM::FACE, CATCHMENT_ATTR, CATCHMENT_LINK) \
        X(WEIR, DM::NODE, WEIR_ATTR, WEIR_LINK)


        #include<viewdefhelper.h> //This would be the header if code completion would work correctly in qtcreator
    }
}

#endif // Sewerdef_H
