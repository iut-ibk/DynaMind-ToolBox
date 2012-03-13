/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2012  Christian Urich
 
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

#ifndef SIMPLESEWERLAYOUT_H
#define SIMPLESEWERLAYOUT_H

#include <dmmodule.h>
#include <dm.h>
class SimpleSewerLayout : public DM::Module
{
    DM_DECLARE_NODE(SimpleSewerLayout)
    private:
        DM::View intersections;
    DM::View streets;
    DM::View conduits;
    DM::View inlets;
    DM::View endnode;
    DM::View catchment;
    DM::View shaft;
    DM::View wwtp;
    DM::View cityblock;
public:
    SimpleSewerLayout();
    DM::Node * connectNextInlet(DM::System *city, DM::Node *n1, int direction);
    void addConduit(DM::System *city, DM::Node *n, int direction);
    void run();
};

#endif // SIMPLESEWERLAYOUT_H
