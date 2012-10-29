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

class ComponentID: public std::pair<std::string,std::pair<std::string,std::string>>
{
public:
	ComponentID(std::string _uuid, std::string _stateUuid, std::string _owner)
	{
		first = _uuid;
		second.first = _stateUuid;
		second.second = _owner;
	}
	ComponentID(DM::Component *c, DM::Component *_owner)
	{
		first = c->getUUID();
		second.first = c->getStateUUID();
		second.second = _owner->getUUID();
	}
	std::string toString()
	{
		std::stringstream stream;
		stream << first;
		stream << "|";
		stream << second.first;
		return stream.str();
	}
	std::string getOwner()
	{
		return second.second;
	}
	std::string getStateUuid()
	{
		return second.first;
	}
	std::string getUuid()
	{
		return first;
	}
};

class ComponentContent
{
public:
	DM::Components	type;
	std::vector<std::string> strvalues;
	std::vector<double> dblValues;
};

typedef std::pair<ComponentID, ComponentContent> idCompPair;
std::map<ComponentID, ComponentContent> mapValidation;

void AddValidation(DM::Component* c, DM::Component* owner)
{
	if(c==NULL)
		DM::Logger(DM::Error) << "Add Validation: NULL pointer parameter";

	DM::Logger(DM::Debug) << "Adding to validation list: " << c->getUUID();

	ComponentID cid(c, owner);

	mapValidation[cid].type = c->getType();
	
	std::vector<std::string> sv;
	std::vector<double> dv;

	switch(c->getType())
	{
	case DM::Components::COMPONENT:
		break;
	case DM::Components::EDGE:
		sv.push_back(((DM::Edge*)c)->getStartpointName());
		sv.push_back(((DM::Edge*)c)->getEndpointName());
		mapValidation[ComponentID(c, owner)].strvalues = sv;
		break;
	case DM::Components::FACE:
		mapValidation[ComponentID(c, owner)].strvalues = ((DM::Face*)c)->getNodes();
		break;
	case DM::Components::NODE:
		dv.push_back(((DM::Node*)c)->get()[0]);
		dv.push_back(((DM::Node*)c)->get()[1]);
		dv.push_back(((DM::Node*)c)->get()[2]);
		mapValidation[ComponentID(c, owner)].dblValues = dv;
		break;
	case DM::Components::RASTERDATA:
		break;
	case DM::Components::SUBSYSTEM:
		break;
	}
}
bool ValidateValues(DM::Component* foundItem, idCompPair item)
{
	ComponentID itemId = item.first;

	bool success = true;
	switch(item.second.type)
	{
	case DM::Components::COMPONENT:
		break;
	case DM::Components::EDGE:
		if(mapValidation[itemId].strvalues[0] != ((DM::Edge*)foundItem)->getStartpointName())
		{
			DM::Logger(DM::Error) << "wrong edge start point " << itemId.toString() 
				<< " is " << ((DM::Edge*)foundItem)->getStartpointName()
				<< " expected " << mapValidation[itemId].strvalues[0];
			success = false;
		}
		if(mapValidation[itemId].strvalues[1] != ((DM::Edge*)foundItem)->getEndpointName())
		{
			DM::Logger(DM::Error) << "wrong edge end point " << itemId.toString()  
				<< " is " << ((DM::Edge*)foundItem)->getEndpointName()
				<< " expected " << mapValidation[itemId].strvalues[1];
			success = false;
		}
		break;
	case DM::Components::FACE:
		for(unsigned int i=0;i<mapValidation[itemId].strvalues.size();i++)
		{
			if(i>((DM::Face*)foundItem)->getNodes().size())
			{
				DM::Logger(DM::Error) << "missing face point ["<<i<<"] " << mapValidation[itemId].strvalues[i];
				success = false;
			}
			else if(mapValidation[itemId].strvalues[i] != ((DM::Face*)foundItem)->getNodes()[i])
			{
				DM::Logger(DM::Error) << "wrong face point ["<<i<<"] " << itemId.toString()  
					<< " is " << ((DM::Edge*)foundItem)->getStartpointName()
					<< " expected " << mapValidation[itemId].strvalues[i];
				success = false;
			}
		}
		break;
	case DM::Components::NODE:
		for(unsigned int i=0;i<mapValidation[itemId].dblValues.size();i++)
		{
			if(((DM::Node*)foundItem)->get()[i] != mapValidation[itemId].dblValues[i])
			{
				DM::Logger(DM::Error) << "wrong coordinate value ["<<i<<"]: is "
					<< ((DM::Node*)foundItem)->get()[i] << " should be " << mapValidation[itemId].dblValues[i];
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
bool Validate(std::map<std::string,DM::Component*> map, DM::Component* owner)
{
	bool success = true;
	
	foreach(idCompPair item, mapValidation)
	{
		ComponentID itemId = item.first;
		if(itemId.getOwner() == owner->getUUID() && itemId.getStateUuid() == owner->getStateUUID())
		{

			std::string strItem = itemId.toString();
		
			DM::Logger(DM::Debug) << "Checking: " << strItem;

			DM::Component* foundItem = map[itemId.getUuid()];

			if(foundItem==NULL)
			{
				DM::Logger(DM::Error) << "missing " << strItem;
				success = false;
			}
			else
			{
				DM::Logger(DM::Debug) << "successfully found " << strItem;
				if(item.second.type != foundItem->getType())
				{
					DM::Logger(DM::Error) << "wrong type " << strItem << " is of type " << foundItem->getType()
						<< "should be " << item.second.type;
					success = false;
				}
				else
				{
					DM::Logger(DM::Debug) << "type is correct " << strItem;
					if(!ValidateValues(foundItem, item))
						success = false;
					else
						DM::Logger(DM::Debug) << "value is correct " << strItem;
				}
			}
		}
	}
	return success;
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
	AddValidation(sys->addEdge(new DM::Edge(x->getUUID(),y->getUUID())), sys);

	std::vector<std::string> nodeVector;
	nodeVector.push_back(x->getUUID());
	nodeVector.push_back(y->getUUID());
	nodeVector.push_back(z->getUUID());
	AddValidation(sys->addFace(new DM::Face(nodeVector)), sys);
	
	DM::RasterData *raster = new DM::RasterData();
	raster->setNoValue(1.0);
	raster->setSize(2.,3.,4.);
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

	if(!Validate(sys->getAllChilds(), sys))
		this->getSimulation()->setSimulationStatus(DM::SIM_FAILED);
}


DM_DECLARE_NODE_NAME(SuccessorCheck, Modules)
SuccessorCheck::SuccessorCheck()
{
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
		this->getSimulation()->setSimulationStatus(DM::SIM_FAILED);
	}
	
	DM::Logger(DM::Debug) << "starting successor validation";
	if(!Validate(sys2->getAllChilds(), sys2))
	{
		DM::Logger(DM::Error) << "successor validation failed";
		this->getSimulation()->setSimulationStatus(DM::SIM_FAILED);
	}
}
