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

/*
#include "createallcomponents.h"
#include <sstream>
#include <dmsimulation.h>
#include <validation.h>

DM_DECLARE_NODE_NAME(CreateAllComponenets, Modules)
CreateAllComponenets::CreateAllComponenets()
{
    std::vector<DM::View> data;
	DM::View comps("Components", DM::COMPONENT, DM::WRITE);
	DM::View nodes("Nodes", DM::NODE, DM::WRITE);
	DM::View faces("Faces", DM::FACE, DM::WRITE);
	DM::View edges("Edges", DM::EDGE, DM::WRITE);
	DM::View rasterdata("Rasterdata", DM::RASTERDATA, DM::WRITE);
	DM::View systems("System", DM::SUBSYSTEM, DM::WRITE);

	comps.addAttribute("yeahttribute");

	data.push_back(comps);
	data.push_back(nodes);
	data.push_back(faces);
	data.push_back(edges);
	data.push_back(rasterdata);
	data.push_back(systems);

    this->addData("sys",data);
}

void CreateAllComponenets::run() 
{	
    DM::Logger(DM::Debug) << "creating components";

    DM::System * sys = this->getData("sys");

	DM::View comps = DM::View("Components", DM::COMPONENT, DM::WRITE);
	DM::View nodes = DM::View("Nodes", DM::NODE, DM::WRITE);
	DM::View faces = DM::View("Faces", DM::FACE, DM::WRITE);
	DM::View edges = DM::View("Edges", DM::EDGE, DM::WRITE);
	DM::View rasterdata = DM::View("Rasterdata", DM::RASTERDATA, DM::WRITE);
	DM::View systems = DM::View("System", DM::SUBSYSTEM, DM::WRITE);

	DM::Node *x = new DM::Node(1,2,3);
	DM::Node *y = new DM::Node(4,5,6);
	DM::Node *z = new DM::Node(7,8,9);

	DM::Component* c = new DM::Component();
	c->addAttribute("yeahttribute", 3.141592);
    AddValidation(sys->addComponent(c), sys);
	AddValidation(sys->addNode(x), sys);
	AddValidation(sys->addNode(y), sys);
	AddValidation(sys->addNode(z), sys);
    AddValidation(sys->addEdge(new DM::Edge(x,y)), sys);

    std::vector<DM::Node*> nodeVector;
    nodeVector.push_back(x);
    nodeVector.push_back(y);
    nodeVector.push_back(z);
    AddValidation(sys->addFace(new DM::Face(nodeVector)), sys);
	
	DM::RasterData *raster = new DM::RasterData();
	raster->setNoValue(1.0);
    raster->setSize(2.,3.,1.,1.,0.,0.);
	raster->setValue(0,0,5.);
	raster->setValue(1,1,6.);
	AddValidation(sys->addRasterData(raster), sys);
	
	DM::System *sub = new DM::System();
	AddValidation(sys->addSubSystem(sub), sys);

    DM::Logger(DM::Debug) << "creating components ... done";
}

DM_DECLARE_NODE_NAME(CheckAllComponenets, Modules)
CheckAllComponenets::CheckAllComponenets()
{
	std::vector<DM::View> data;
	DM::View comps("Components", DM::COMPONENT, DM::READ);
	DM::View nodes("Nodes", DM::NODE, DM::READ);
	DM::View faces("Faces", DM::FACE, DM::READ);
	DM::View edges("Edges", DM::EDGE, DM::READ);
	DM::View rasterdata("Rasterdata", DM::RASTERDATA, DM::READ);
	DM::View systems = DM::View("System", DM::SUBSYSTEM, DM::WRITE);
	comps.addAttribute("yeahttribute");

	data.push_back(comps);
	data.push_back(nodes);
	data.push_back(faces);
	data.push_back(edges);
	data.push_back(rasterdata);
	data.push_back(systems);

    this->addData("sys",data);
}

void CheckAllComponenets::run() {

    DM::Logger(DM::Debug) << "checking components";

    DM::System * sys = this->getData("sys");
	success = Validate(sys->getAllChilds(), sys);
}


DM_DECLARE_NODE_NAME(SuccessorCheck, Modules)
SuccessorCheck::SuccessorCheck()
{
	success = true;
	std::vector<DM::View> data;
	DM::View comps("Components", DM::COMPONENT, DM::MODIFY);
	DM::View nodes("Nodes", DM::NODE, DM::MODIFY);
	DM::View faces("Faces", DM::FACE, DM::MODIFY);
	DM::View edges("Edges", DM::EDGE, DM::MODIFY);
	DM::View rasterdata("Rasterdata", DM::RASTERDATA, DM::MODIFY);
	DM::View systems("System", DM::SUBSYSTEM, DM::WRITE);
	comps.addAttribute("yeahttribute");

	data.push_back(comps);
	data.push_back(nodes);
	data.push_back(faces);
	data.push_back(edges);
	data.push_back(rasterdata);
	data.push_back(systems);

    this->addData("sys",data);
	success = true;
}

typedef std::pair<std::string, DM::Node*> uuidNodePair;
typedef std::pair<std::string, DM::Component*> uuidCompPair;

void SuccessorCheck::run() 
{
    DM::System * sys = this->getData("sys");
	
	DM::Logger(DM::Debug) << "generating successor state";
	DM::System * sys2 = sys->createSuccessor();
	foreach(uuidCompPair p,sys2->getAllChilds())
	{
		AddValidation(p.second, sys2);
	}

	// VALIDATE
	DM::Logger(DM::Debug) << "starting system validation";
	if(!Validate(sys->getAllChilds(), sys))
	{
		DM::Logger(DM::Error) << "system validation failed";
		success = false;
		return;
	}
	
	DM::Logger(DM::Debug) << "starting successor validation";
	if(!Validate(sys2->getAllChilds(), sys2))
	{
		DM::Logger(DM::Error) << "successor validation failed";
		success = false;
		return;
	}
}
*/
