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

#ifndef ExtractNodesFromEdges_H
#define ExtractNodesFromEdges_H

#include <dmmodule.h>
#include <dm.h>
#include <graphviewdef.h>

class ExtractNodesFromEdges : public DM::Module
{
	DM_DECLARE_NODE(ExtractNodesFromEdges)

private:
	typedef std::map<DM::GRAPH::COMPONENTS,DM::View> viewmap;

	DM::System *sys;
	viewmap viewdef;
	bool skeletonize;

public:
	ExtractNodesFromEdges();

	void run();
	void initmodel(){}
	bool checkNode(DM::Node* node, std::vector<DM::Component*> &edges,bool skeletonize);
};

#endif // ExtractNodesFromEdges_H
