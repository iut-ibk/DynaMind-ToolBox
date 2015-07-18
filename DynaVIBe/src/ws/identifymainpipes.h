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

#ifndef identifyMainPipes_H
#define identifyMainPipes_H

#include <dmmodule.h>
#include <dm.h>

#include <watersupplyviewdef.h>

#include <dynamindboostgraphhelper.h>

class IdentifyMainPipes : public DM::Module
{
	DM_DECLARE_NODE(IdentifyMainPipes)

private:
	DM::System *sys;
	DM::WS::ViewDefinitionHelper defhelper_ws;

public:
	IdentifyMainPipes();

	void run();
	void initmodel();

private:
	std::vector<DM::Edge*> getEdgesInView(DM::Node* node, std::vector<DM::Component*> &edges);
	bool removeLeafs(std::vector<DM::Component*> nodes, std::vector<DM::Component*> edges, std::vector<DM::Component*> &resultedges, std::vector<DM::Component*> &resultnodes);
	bool addPathToNode(DM::Node* rootnode,std::vector<DM::Component *> nodes, std::vector<DM::Component *> edges, std::vector<DM::Component *> &resultedges, std::vector<DM::Component*> &resultnodes);
};

#endif // identifyMainPipes_H
