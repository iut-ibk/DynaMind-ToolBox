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

#include <dmcomponent.h>
#include <dmedge.h>

using namespace DM;

Edge::Edge(std::string startpoint, std::string endpoint) : Component()
{
    start=startpoint;
    end=endpoint;
}

Edge::Edge(const Edge& e) : Component(e)
{
    start=e.start;
    end=e.end;
}

std::string Edge::getStartpointName()
{
    return start;
}

std::string Edge::getEndpointName()
{
    return end;
}

void Edge::setStartpointName(std::string name) {
    this->start = name;
}

void Edge::setEndpointName(std::string name) {
    this->end = name;
}

Component* Edge::clone()
{
    return new Edge(*this);
}

