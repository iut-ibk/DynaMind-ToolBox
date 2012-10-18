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

#include "createallcomponents.h"
#include <sstream>
#include <dmsimulation.h>

DM_DECLARE_NODE_NAME(CreateAllComponenets, Modules)
CreateAllComponenets::CreateAllComponenets()
{
    std::vector<DM::View> data;
	DM::View comps("Components", DM::COMPONENT, DM::WRITE);
	DM::View nodes("Nodes", DM::NODE, DM::WRITE);
	DM::View faces("Faces", DM::FACE, DM::WRITE);
	DM::View edges("Edges", DM::EDGE, DM::WRITE);
	DM::View rasterdata("Rasterdata", DM::RASTERDATA, DM::WRITE);
	//TODO nodes.addAttribute("nodeattribute");

	data.push_back(comps);
	data.push_back(nodes);
	data.push_back(faces);
	data.push_back(edges);
	data.push_back(rasterdata);

    this->addData("sys",data);
}

// ugly globals
typedef std::pair<std::string, int> uuidCompPair;
std::map<std::string, int> itemMap;
std::map<std::string, std::vector<std::string>> strValueMap;
std::map<std::string, std::vector<double>> dblValueMap;

void AddValidation(DM::Component* c)
{
	if(c==NULL)
		DM::Logger(DM::Error) << "Add Validation: NULL pointer parameter";

	DM::Logger(DM::Debug) << "Adding to validation list: " << c->getUUID();

	itemMap[c->getUUID()] = (int)c->getType();
	
	std::vector<std::string> sv;
	std::vector<double> dv;

	switch(c->getType())
	{
	case DM::Components::COMPONENT:
		break;
	case DM::Components::EDGE:
		sv.push_back(((DM::Edge*)c)->getStartpointName());
		sv.push_back(((DM::Edge*)c)->getEndpointName());
		strValueMap[c->getUUID()] = sv;
		break;
	case DM::Components::FACE:
		strValueMap[c->getUUID()] = ((DM::Face*)c)->getNodes();
		break;
	case DM::Components::NODE:
		dv.push_back(((DM::Node*)c)->get()[0]);
		dv.push_back(((DM::Node*)c)->get()[1]);
		dv.push_back(((DM::Node*)c)->get()[2]);
		dblValueMap[c->getUUID()] = dv;
		break;
	case DM::Components::RASTERDATA:
		break;
	case DM::Components::SUBSYSTEM:
		break;
	}
}


bool ValidateValues(DM::Component* foundItem, std::string itemUuid, DM::Components itemType, std::string strItem)
{
	bool success = true;
	switch(itemType)
	{
	case DM::Components::COMPONENT:
		break;
	case DM::Components::EDGE:
		if(strValueMap[itemUuid][0] != ((DM::Edge*)foundItem)->getStartpointName())
		{
			DM::Logger(DM::Error) << "wrong edge start point " << strItem 
				<< " is " << ((DM::Edge*)foundItem)->getStartpointName()
				<< " expected " << strValueMap[itemUuid][0];
			success = false;
		}
		if(strValueMap[itemUuid][1] != ((DM::Edge*)foundItem)->getEndpointName())
		{
			DM::Logger(DM::Error) << "wrong edge end point " << strItem 
				<< " is " << ((DM::Edge*)foundItem)->getEndpointName()
				<< " expected " << strValueMap[itemUuid][1];
			success = false;
		}
		break;
	case DM::Components::FACE:
		for(unsigned int i=0;i<strValueMap[itemUuid].size();i++)
		{
			if(i>((DM::Face*)foundItem)->getNodes().size())
			{
				DM::Logger(DM::Error) << "missing face point ["<<i<<"] " << strValueMap[itemUuid][i];
				success = false;
			}
			else if(strValueMap[itemUuid][i] != ((DM::Face*)foundItem)->getNodes()[i])
			{
				DM::Logger(DM::Error) << "wrong face point ["<<i<<"] " << strItem 
					<< " is " << ((DM::Edge*)foundItem)->getStartpointName()
					<< " expected " << strValueMap[itemUuid][i];
				success = false;
			}
		}
		break;
	case DM::Components::NODE:
		for(int i=0;i<dblValueMap[itemUuid].size();i++)
		{
			if(((DM::Node*)foundItem)->get()[i] != dblValueMap[itemUuid][i])
			{
				DM::Logger(DM::Error) << "wrong coordinate value ["<<i<<"]: is "
					<< ((DM::Node*)foundItem)->get()[i] << " should be " << dblValueMap[itemUuid][i];
				success = false;
			}
		}
		break;
	case DM::Components::RASTERDATA:
		break;
	case DM::Components::SUBSYSTEM:
		break;
	}
	return success;
}

bool Validate(std::map<std::string,DM::Component*> map)
{
	bool success = true;

	foreach(uuidCompPair item, itemMap)
	{
		std::string itemUuid = item.first;
		DM::Components itemType = (DM::Components)item.second;
		
		DM::Logger(DM::Debug) << "Checking: " << itemUuid;

		std::stringstream strstrItem;
		strstrItem << "{ uuid="<<itemUuid<< "\ttype="<<(int)itemType<<" }";
		std::string strItem = strstrItem.str();

		DM::Component* foundItem = map[itemUuid];

		if(foundItem==NULL)
		{
			DM::Logger(DM::Error) << "missing " << strItem;
			success = false;
		}
		else
		{
			DM::Logger(DM::Debug) << "successfully found " << strItem;
			if(item.second != foundItem->getType())
			{
				DM::Logger(DM::Error) << "wrong type " << strItem << " is of type " << foundItem->getType();
				success = false;
			}
			else
			{
				DM::Logger(DM::Debug) << "type is correct " << strItem;
				if(!ValidateValues(foundItem, itemUuid, itemType, strItem))
					success = false;
				else
					DM::Logger(DM::Debug) << "value is correct " << itemUuid;
			}
		}
	}
	return success;
}

void CreateAllComponenets::run() {

    DM::Logger(DM::Debug) << "creating components";

    DM::System * sys = this->getData("sys");

	DM::View comps = DM::View("Components", DM::COMPONENT, DM::WRITE);
	DM::View nodes = DM::View("Nodes", DM::NODE, DM::WRITE);
	DM::View faces = DM::View("Faces", DM::FACE, DM::WRITE);
	DM::View edges = DM::View("Edges", DM::EDGE, DM::WRITE);
	DM::View rasterdata = DM::View("Rasterdata", DM::RASTERDATA, DM::WRITE);

	DM::Node *x = new DM::Node(1,2,3);
	DM::Node *y = new DM::Node(4,5,6);
	DM::Node *z = new DM::Node(7,8,9);
	
	AddValidation(sys->addComponent(new DM::Component()));
	AddValidation(sys->addNode(x));
	AddValidation(sys->addNode(y));
	AddValidation(sys->addNode(z));
	AddValidation(sys->addEdge(new DM::Edge(x->getUUID(),y->getUUID())));

	std::vector<std::string> nodeVector;
	nodeVector.push_back(x->getUUID());
	nodeVector.push_back(y->getUUID());
	nodeVector.push_back(z->getUUID());
	AddValidation(sys->addFace(new DM::Face(nodeVector)));

	DM::RasterData *raster = new DM::RasterData();
	raster->setNoValue(1.0);
	raster->setSize(2.,3.,4.);
	raster->setValue(0,0,5.);
	raster->setValue(1,1,6.);
	AddValidation(sys->addRasterData(raster));

	DM::System *sub = new DM::System();
	AddValidation(sys->addSubSystem(sub));

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

/*
void CheckCount(int expected, int measured, std::string component)
{
	if(expected==measured)	
		DM::Logger(DM::Debug) << "successfully found all " << component << " (" << QString(measured) << ")";
	else DM::Logger(DM::Error) << "missing " << component << " (" << QString(measured) << " of " << QString(expected) << ")";
}
*/

void CheckAllComponenets::run() {

    DM::Logger(DM::Debug) << "checking components";

    DM::System * sys = this->getData("sys");

	/*
	CheckCount(1, sys->getAllComponents().size(), "components");
	CheckCount(3, sys->getAllNodes().size(), "nodes");
	CheckCount(1, sys->getAllEdges().size(), "edges");
	CheckCount(1, sys->getAllFaces().size(), "faces");
	// TODO check rasterdata 
	*/
	if(!Validate(sys->getAllOwnedChilds()))
		this->getSimulation()->setSimulationStatus(DM::SIM_FAILED);
}
