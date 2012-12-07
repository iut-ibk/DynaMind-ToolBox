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

#ifndef Graphdef_H
#define Graphdef_H

#include <dm.h>
#include <string.h>

namespace DM
{
    namespace GRAPH
    {
        #define EDGES_LINK
        #define EDGES_ATTR \
        A(Weight)

        #define NODES_LINK
        #define NODES_ATTR

        #define TABLE \
        X(NODES,DM::NODE, NODES_ATTR, NODES_LINK) \
        X(EDGES,DM::EDGE,EDGES_ATTR, EDGES_LINK) \
        X(SPANNINGTREE,DM::EDGE,EDGES_ATTR,EDGES_LINK)

        #include<viewdefhelper.h>
    }
}

#endif // Graphdef_H
