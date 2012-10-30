
#include <validation.h>
#include <QtCore>
#include <sstream>
#include <dmsystem.h>
#include <dmedge.h>
#include <dmnode.h>
#include <dmface.h>
#include <dmrasterdata.h>

ComponentID::ComponentID(std::string _uuid, std::string _stateUuid, std::string _owner)
{
	first = _uuid;
	second.first = _stateUuid;
	second.second = _owner;
}
ComponentID::ComponentID(DM::Component *c, DM::Component *_owner)
{
	first = c->getUUID();
	second.first = c->getStateUUID();
	second.second = _owner->getUUID();
}
std::string ComponentID::toString()
{
	std::stringstream stream;
	stream << first;
	stream << "|";
	stream << second.first;
	return stream.str();
}
std::string ComponentID::getOwner()
{
	return second.second;
}
std::string ComponentID::getStateUuid()
{
	return second.first;
}
std::string ComponentID::getUuid()
{
	return first;
}


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
	case DM::COMPONENT:
		break;
	case DM::EDGE:
		sv.push_back(((DM::Edge*)c)->getStartpointName());
		sv.push_back(((DM::Edge*)c)->getEndpointName());
		mapValidation[ComponentID(c, owner)].strvalues = sv;
		break;
	case DM::FACE:
		mapValidation[ComponentID(c, owner)].strvalues = ((DM::Face*)c)->getNodes();
		break;
	case DM::NODE:
		dv.push_back(((DM::Node*)c)->getX());
		dv.push_back(((DM::Node*)c)->getY());
		dv.push_back(((DM::Node*)c)->getZ());
		mapValidation[ComponentID(c, owner)].dblValues = dv;
		break;
	case DM::RASTERDATA:
		break;
	case DM::SUBSYSTEM:
		break;
	}
}
bool ValidateValues(DM::Component* foundItem, idCompPair item)
{
	ComponentID itemId = item.first;

	bool success = true;
	switch(item.second.type)
	{
	case DM::COMPONENT:
		break;
	case DM::EDGE:
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
	case DM::FACE:
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
	case DM::NODE:
		for(unsigned int i=0;i<mapValidation[itemId].dblValues.size();i++)
		{
			if(((DM::Node*)foundItem)->get(i) != mapValidation[itemId].dblValues[i])
			{
				DM::Logger(DM::Error) << "wrong coordinate value ["<<i<<"]: is "
					<< ((DM::Node*)foundItem)->get(i) << " should be " << mapValidation[itemId].dblValues[i];
				success = false;
			}
		}
		break;
	case DM::RASTERDATA:
		break;
	case DM::SUBSYSTEM:
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
