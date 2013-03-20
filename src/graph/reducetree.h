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

#ifndef ReduceTree_H
#define ReduceTree_H

#include <dmmodule.h>
#include <dm.h>

//Viewdef
#include <graphviewdef.h>


class ReduceTree : public DM::Module
{
    DM_DECLARE_NODE(ReduceTree)

private:
    typedef std::map<DM::GRAPH::COMPONENTS,DM::View> viewmap;

    DM::System *sys;
    viewmap viewdef;

public:
    ReduceTree();

    void run();
    bool deleteflagrecursive(std::vector<std::string> &edges, std::vector<std::string> &forcednodes, std::string currentnode, std::vector<std::string> &visitednodes, std::map<std::string, bool> &deletenodes);
    void deleteflagiterative(std::vector<std::string> &edges, std::vector<std::string> &forcednodes, std::vector<std::string> &deleteedges);
    void initmodel(){}

private:
    std::vector<DM::Edge*> getEdgesInView(DM::Node* node, std::map<std::string, DM::Component*> &edges);
};

#endif // ReduceTree_H
