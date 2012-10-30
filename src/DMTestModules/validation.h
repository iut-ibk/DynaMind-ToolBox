
#ifndef VALIDATION_H
#define VALIDATION_H

#include <dmsimulation.h>
#include <dmmodule.h>
#include <dmsystem.h>

void AddValidation(DM::Component* c, DM::Component* owner);
bool Validate(std::map<std::string,DM::Component*> map, DM::Component* owner);

class ComponentID: public std::pair<std::string,std::pair<std::string,std::string>>
{
public:
	ComponentID(std::string _uuid, std::string _stateUuid, std::string _owner);
	ComponentID(DM::Component *c, DM::Component *_owner);
	std::string toString();
	std::string getOwner();
	std::string getStateUuid();
	std::string getUuid();
};

class ComponentContent
{
public:
	DM::Components	type;
	std::vector<std::string> strvalues;
	std::vector<double> dblValues;
};

#endif // VALIDATION_H
