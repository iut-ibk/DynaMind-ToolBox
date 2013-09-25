/**
* @file
* @author  Markus Sengthaler <m.sengthaler@gmail.com>
* @version 1.0
* @section LICENSE
*
* This file is part of DynaMind
*
* Copyright (C) 2013  Markus Sengthaler

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

#include "containergroup.h"
#include "guicontainergroup.h"
#include <dmlogger.h>

using namespace DM;

DM_DECLARE_NODE_NAME(ContainerGroup, Groups)
	
ContainerGroup::ContainerGroup() 
{
	this->addParameter("inStreams", DM::STRING_LIST, &inStreams);
	this->addParameter("outStreams", DM::STRING_LIST, &outStreams);
	this->addParameter("parameterConfig", DM::STRING_MAP, &parameterConfig);
}

void ContainerGroup::run() 
{
}

void ContainerGroup::init() 
{
	foreach(std::string streamName, outStreams)
		if(!initStream(streamName, true, true))
			setOutPortData(streamName, NULL);

	foreach(std::string streamName, inStreams)
		initStream(streamName, false, true);
}

bool ContainerGroup::addStream(std::string name, bool out)
{
	if(initStream(name, out))
	{
		if(out)
			outStreams.push_back(name);
		else
			inStreams.push_back(name);
		return true;
	}
	else
		return false;
}

bool ContainerGroup::initStream(std::string name, bool out, bool silent)
{
	if(hasInPort(name) || hasOutPort(name))
	{
		if(!silent)
			DM::Logger(Error) << "port already existent";
		return false;
	}

	if(out)
		addPort(name, OUTPORT);
	else
		addPort(name, INPORT);

	return true;
}

bool ContainerGroup::removeStream(std::string name)
{
	std::vector<std::string>::iterator it = find(outStreams.begin(), outStreams.end(), name);
	if(it != outStreams.end())
	{
		outStreams.erase(it);
		removePort(name, INPORT);
		return true;
	}

	it = find(inStreams.begin(), inStreams.end(), name);
	if(it != inStreams.end())
	{
		inStreams.erase(it);
		removePort(name, INPORT);
		return true;
	}
	return false;
}

bool ContainerGroup::createInputDialog() 
{
	QWidget * w = new GUIContainerGroup(this);
	w->show();
	return true;
}
