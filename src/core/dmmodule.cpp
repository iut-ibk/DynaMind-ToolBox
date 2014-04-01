/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @author  Markus Sengthaler <m.sengthaler@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2011  Christian Urich, Markus Sengthaler

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

#include "dmmodule.h"
#include "dmmoduleobserver.h"
#include <sstream>
#include <dmsystem.h>
#include <dmrasterdata.h>
#include "dmlogger.h"

using namespace std;
using namespace DM;

Module::Module()
{
	status = MOD_UNTOUCHED;
	owner = NULL;
	successorMode = false;
}
Module::~Module()
{
	foreach(Parameter* p, parameters)
		delete p;

	parameters.clear();

	// delete systems which end in this module
	mforeach(System* indata, inPorts)
	{
		mforeach(System* outdata, outPorts)
			if(indata == outdata)
				indata = NULL;

		if(indata)
			delete indata;
	}

	foreach(ModuleObserver* observer, observers)
		delete observer;
	observers.clear();
}
void Module::addParameter(const std::string &name, DataTypes type, void * ref, std::string description) 
{
	parameters.push_back(new Parameter(name, type, ref, description));
}

Module::Parameter* Module::getParameter(const std::string& name) const
{
	foreach(Parameter* p, parameters)
		if(p->name == name)
			return p;
	return NULL;
}

std::vector<Module::Parameter*> Module::getParameters() const
{
	return parameters;
}

void Module::setParameterValue(const std::string& name, const std::string& value)
{
	QString qvalue = QString::fromStdString(value);
	Parameter* p = getParameter(name);
	if(!p)
		return;
	switch(p->type)
	{
	case DM::INT:		*(int*)		p->data = qvalue.toInt();	break;
	case DM::LONG:		*(long*)	p->data = qvalue.toLong();	break;
	case DM::DOUBLE:	*(double*)	p->data = qvalue.toDouble();break;
	case DM::BOOL:		*(bool*)	p->data = qvalue.toInt();	break;
	case DM::FILENAME:	
	case DM::STRING:	*(std::string*)	p->data = value;	break;
	case DM::STRING_LIST:	
		{
			std::vector<std::string> v;
			// foreach will cause a compile error in modules not including qt headers
			QVector<QString> qvalues = qvalue.split("*|*", QString::SkipEmptyParts).toVector();
			for(QVector<QString>::iterator it = qvalues.begin(); it != qvalues.end(); ++it)
				if(it->size())
					v.push_back(it->toStdString());
			*(std::vector<std::string>*)p->data = v;
		}
		break;
	case DM::STRING_MAP:	
		{
			std::map<std::string,std::string> map;
			QVector<QString> rows = qvalue.split("*||*").toVector();
			foreach(QString rowString, rows)
			{
				QVector<QString> keyValuePair =  rowString.split("*|*").toVector();
				if (!(keyValuePair.first().isEmpty() && keyValuePair.last().isEmpty()))	// if key and value are empty, skip
					map[keyValuePair.first().toStdString()] = keyValuePair.last().toStdString();
			}
			*(std::map<std::string,std::string>*)p->data = map;
		}
		break;
	}
}

void Module::addObserver(ModuleObserver* obs)
{
	observers.push_back(obs);
}
void Module::removeObserver(ModuleObserver* obs)
{
	remove(observers.begin(), observers.end(), obs);
}
void Module::removeAllObservers()
{
	foreach(ModuleObserver* obs, observers)
		delete obs;
	observers.clear();
}

std::string Module::getName() const
{
	return name.length()>0 ? name : std::string("<class: ") + getClassName() + ">";
}

void Module::setName(std::string name)
{
	this->name = name;

	foreach(ModuleObserver* observer, observers)
		observer->notifyChangeName(name);
}

void Module::addPort(const std::string &name, const PortType type)
{
	if(type == INPORT)
		inPorts[name] = NULL;
	else if(type == OUTPORT)
		outPorts[name] = NULL;

	foreach(ModuleObserver* observer, observers)
		observer->notifyAddPort(name, type);
}

void Module::removePort(const std::string &name, const PortType type)
{
	if(type == INPORT)
		inPorts.erase(name);
	else if(type == OUTPORT)
		outPorts.erase(name);

	foreach(ModuleObserver* observer, observers)
		observer->notifyRemovePort(name, type);
}
bool Module::hasInPort(const std::string &name) const
{
	return map_contains(&inPorts, name);
}
bool Module::hasOutPort(const std::string &name) const
{
	return map_contains(&outPorts, name);
}

void Module::setInPortData(const std::string &name, System* data)
{
	if(!map_contains(&inPorts, name))
		DM::Logger(Error) << "accessing non existent in port '" << name << "' in module '" 
		<< this->getClassName() << "', canceled";
	else
		inPorts[name] = data;
}

void Module::setOutPortData(const std::string &name, System* data)
{
	if(!map_contains(&outPorts, name))
		DM::Logger(Error) << "accessing non existent out port '" << name << "' in module '" 
		<< this->getClassName() << "', canceled";
	else
		outPorts[name] = data;
}

bool Module::inPortsSet() const
{
	mforeach(System* data, inPorts)
		if(!data)
			return false;
	return true;
}
bool Module::outPortsSet() const
{
	mforeach(System* data, outPorts)
		if(!data)
			return false;
	return true;
}

System* Module::getInPortData(const std::string &name) const
{
	System* sys = NULL;
	map_contains(&inPorts, name, sys);
	return sys;
}
System* Module::getOutPortData(const std::string &name) const
{
	System* sys = NULL;
	map_contains(&outPorts, name, sys);
	return sys;
}

void Module::addData(const std::string& streamName, std::vector<View> views)
{
	if(views.size())
	{
		std::map<std::string,View> viewMap;
		foreach(const View v, views)
			viewMap[v.getName()] = v;

		this->accessedViews[streamName] = viewMap;
	}
	else
		this->accessedViews.erase(streamName);

	bool inPort = false;
	bool outPort = false;

	foreach(View v, views)
	{
		//ACCESS a = (ACCESS)v.getAccessType();
		//if(a == READ || a == MODIFY || v.getReadAttributes().size())
		if(v.reads())
			inPort = true;
		//if(a == WRITE || a == MODIFY || v.getWriteAttributes().size())
		if(v.writes())
			outPort = true;
	}

	if(inPort && !hasInPort(streamName))
		this->addPort(streamName, INPORT);
	else if (!inPort && hasInPort(streamName))
		this->removePort(streamName, INPORT);

	if(outPort && !hasOutPort(streamName))
		this->addPort(streamName, OUTPORT);
	else if (!outPort && hasOutPort(streamName))
		this->removePort(streamName, OUTPORT);
}

void Module::removeData(const std::string& name)
{
	accessedViews.erase(name);

	if(hasInPort(name))
		this->removePort(name, INPORT);
	if(hasOutPort(name))
		this->removePort(name, OUTPORT);
}

System* Module::getData(const std::string& streamName)
{
	if(!hasInPort(streamName) && !hasOutPort(streamName))
	{
		Logger(Error) << "stream '" << streamName << "' does not exist in module '" << this->getClassName();
		return NULL;
	}

	System *sys = getInPortData(streamName);
	if(!sys)
	{
		if(this->getInPortNames().size() != 0)
		{
			bool emptyInPorts = true;
			mforeach(System* s, inPorts)
				if(s)
				{
					emptyInPorts = false;
					break;
				}

			if(emptyInPorts)
			{
				// we didn't get a system, but we all ports are empty -> simulation not ready
				// this can happen if module::init calles getData, which is deprecated
				DM::Logger(Error) << "module '" << getClassName() << "' may calls getData('"
					<< streamName << "') while initializing, "
					<< "please use getViewsInStream to retrieve the desired views from stream";
				return NULL;
			}
		}

		if(hasOutPort(streamName))	// maybe the system is already created during the run
			sys = getOutPortData(streamName);

		if(!sys)
			sys = new System();
	}

	if (DBConnector::getInstance()->getConfig().peterDatastream)
	{
		Logger(Debug) << "moving system into db";
		sys->_moveToDb();
		Logger(Debug) << "moving system into db finished";
	}

	std::vector<View> views;
	mforeach(const View& v, accessedViews[streamName])
		views.push_back(v);

	sys->updateViews(views);
	//mforeach(View v, accessedViews[streamName])
	//	sys->updateView(v);

	if (DBConnector::getInstance()->getConfig().peterDatastream)
	{
		Logger(Debug) << "importing system from db";
		sys->_importViewElementsFromDB();
		Logger(Debug) << "importing system from db finished";
	}

	if(hasOutPort(streamName))
		this->setOutPortData(streamName, sys);

	return sys;
}

RasterData* Module::getRasterData(std::string name, View view)
{
	System* data = getData(name);
	if(!data)
	{
		DM::Logger(Error) << "getRasterData: data stream '" << name << "' not found";
		return NULL;
	}
	if(view.getAccessType() == WRITE)
		return data->addRasterData(new RasterData(), view);
	else
	{
		const std::vector<Component*>& comps = data->getAllComponentsInView(view);
		if(comps.size()>1)
			DM::Logger(Warning) << "View contains more than one RasterData set,\
									may lead to corrupted data stream";
		foreach(Component* c, data->getAllComponentsInView(view))
			if(c->getType() == RASTERDATA)
				return (RasterData*)c;
		DM::Logger(Error) << "getRasterData: rasterdata in view '" << view.getName() << "' not found";
		return NULL;
	}
}

std::map<std::string, std::map<std::string,View> > Module::getAccessedViews() const
{
	return accessedViews;
}
std::map<std::string, std::map<std::string, View> > Module::getViewsInStream() const
{
	return streamViews;
}
std::map<std::string, std::map<std::string, View> > Module::getViewsInOutStream() const
{
	return outStreamViews;
}

std::vector<View> Module::getViewsInStream(const std::string& streamName) const
{
	std::vector<View> views;
	std::map<std::string, View> r;
	if(map_contains(&streamViews, streamName, r))
		mforeach(const View& v, r)
		views.push_back(v);

	return views;
}


View Module::getViewInStream(const std::string& streamName, const std::string& viewName) const
{
	std::map<std::string,View> views;
	View view;
	if(map_contains(&streamViews, streamName, views))
		if(map_contains(&views, viewName, view))
			return view;

	return view;
}

std::map<std::string,View> Module::getViewsInStdStream() const
{
	if(streamViews.size() > 0)
		return (streamViews.begin())->second;
	else
		return std::map<std::string,View>();
}

std::string Module::getUuid() const
{
	Logger(Warning) << "module::getUuid() deprecated";
	return "<Module::getUuid deprecated>";
}

std::map<std::string, std::map<std::string, DM::View> > Module::getViews() const
{
	return getAccessedViews();
}

void Module::setStatus(ModuleStatus status)
{
	this->status = status;
	foreach(ModuleObserver* obs, observers)
		obs->notifyStateChange();
}

void Module::setOwner(Module* owner)
{
	if(!this->owner)
		this->owner = owner;
	else
		Logger(Error) << "reassigning a new parent to a module is not allowed";
}

void Module::setSuccessorMode(bool value)
{
	Logger(Debug) << "changed successor mode state of module '" 
		<< getClassName() << "' to " << (value?"ON":"OFF");
	this->successorMode = value;
}
bool Module::isSuccessorMode() const
{
	return successorMode;
}

std::string Module::getParameterAsString(const std::string& name) const
{   
	std::stringstream strValue;
	strValue.precision(16);

	if(Parameter* p = getParameter(name))
	{
		switch(p->type)
		{
		case DOUBLE:	strValue << *(double*)p->data;	break;
		case INT:		strValue << *(int*)p->data;	break;
		case LONG:		strValue << *(long*)p->data;	break;
		case BOOL:		strValue << *(bool*)p->data;	break;
		case FILENAME:
		case STRING:	strValue << *(std::string*)p->data;	break;
		case STRING_LIST:
			foreach(std::string s, *(std::vector<std::string>*)p->data)
				strValue << s << "*|*" ;
			break;
		case STRING_MAP:
			{
				std::map<std::string, std::string> map = *(std::map<std::string,std::string>*)p->data;
				for (std::map<std::string, std::string>::iterator it = map.begin(); it != map.end(); ++it)
					strValue << it->first << "*|*" << it->second << "*||*" ;
				break;
			}
		}
	}
	return strValue.str();
}

std::vector<std::string> Module::getInPortNames() const
{
	std::vector<std::string> list;
	std::map<std::string, System*>::const_iterator it = inPorts.begin();
	for(;it != inPorts.end(); ++it)
		list.push_back(it->first);
	return list;
}
std::vector<std::string> Module::getOutPortNames() const
{
	std::vector<std::string> list;
	std::map<std::string, System*>::const_iterator it = outPorts.begin();
	for(;it != outPorts.end(); ++it)
		list.push_back(it->first);
	return list;
}

void Module::reset()
{
	streamViews.clear();

	for(std::map<std::string, System*>::iterator it = inPorts.begin(); it != inPorts.end(); ++it)
		if(it->second)
			it->second = NULL;

	for(std::map<std::string, System*>::iterator it = outPorts.begin(); it != outPorts.end(); ++it)
		it->second = NULL;

	this->setStatus(MOD_UNTOUCHED);
}
