/**
 * @file
 * @author  Markus Sengthaler <m.sengthaler@gmail.com>
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


#include "reallocator.h"

DM_DECLARE_NODE_NAME(Reallocator, Modules)
Reallocator::Reallocator()
{
	std::vector<DM::View> data;
	DM::View comps("Components", DM::COMPONENT, DM::MODIFY);
	DM::View nodes("Nodes", DM::NODE, DM::MODIFY);
	DM::View faces("Faces", DM::FACE, DM::MODIFY);
	DM::View edges("Edges", DM::EDGE, DM::MODIFY);
	DM::View rasterdata("Rasterdata", DM::RASTERDATA, DM::MODIFY);
	//nodes.addAttribute("nodeattribute");

	data.push_back(comps);
	data.push_back(nodes);
	data.push_back(faces);
	data.push_back(edges);
	data.push_back(rasterdata);

    this->addData("sys",data);

	//nodes.getAttribute("nodeattribute");
	//data
	//	this.addData();
}

void Reallocator::run() 
{
    DM::System * sys = this->getData("sys");
	
    DM::Logger(DM::Debug) << "unload system entries";
	sys->ForceDeallocation();
    DM::Logger(DM::Debug) << "load system entries";
	sys->ForceAllocation();
	
    DM::Logger(DM::Debug) << "finished reallocation module run";
}

