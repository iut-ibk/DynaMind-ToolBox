/**
* @file
* @author  Chrisitan Urich <christian.urich@gmail.com>
* @version 1.0
* @section LICENSE
*
* This file is part of DynaMind
*
* Copyright (C) 2011  Christian Urich

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

#include "loopgroup.h"
#include "guiloopgroup.h"
#include <dmlogger.h>

using namespace DM;

DM_DECLARE_NODE_NAME(LoopGroup, Groups)

LoopGroup::LoopGroup() 
{
	runs = 1;
	currentRun = 0;

	this->addParameter("Runs", DM::INT, &runs);
	this->addParameter("writeStreams", DM::STRING_LIST, &writeStreams);
	this->addParameter("readStreams", DM::STRING_LIST, &readStreams);
}

void LoopGroup::run() 
{
}

int LoopGroup::getGroupCounter()
{
	return currentRun;
}

void LoopGroup::init() 
{
	currentRun = 0;

	foreach(std::string streamName, writeStreams)
		if(!initStream(streamName, true, true))
			setOutPortData(streamName, NULL);

	foreach(std::string streamName, readStreams)
		initStream(streamName, false, true);
}

bool LoopGroup::addStream(std::string name, bool write)
{
	if(initStream(name, write))
	{
		if(write)
			writeStreams.push_back(name);
		else
			readStreams.push_back(name);
		return true;
	}
	else
		return false;
}

bool LoopGroup::initStream(std::string name, bool write, bool silent)
{
	if(hasInPort(name) || hasOutPort(name))
	{
		if(!silent)
			DM::Logger(Error) << "port already existent";
		return false;
	}

	addPort(name, INPORT);
	if(write)
		addPort(name, OUTPORT);

	return true;
}

bool LoopGroup::removeStream(std::string name)
{
	std::vector<std::string>::iterator it = find(writeStreams.begin(), writeStreams.end(), name);
	if(it != writeStreams.end())
	{
		writeStreams.erase(it);
		removePort(name, INPORT);
		removePort(name, OUTPORT);
		return true;
	}

	it = find(readStreams.begin(), readStreams.end(), name);
	if(it != readStreams.end())
	{
		readStreams.erase(it);
		removePort(name, INPORT);
		return true;
	}
	return false;
}

bool LoopGroup::condition()
{
	if(currentRun++ < runs)
	{
		loopStreams();
		return true;
	}
	return false;
}

void LoopGroup::loopStreams()
{
	foreach(std::string streamName, writeStreams)
	{
		if(ISystem* sys = getOutPortData(streamName))
		{
			setInPortData(streamName, sys);
			setOutPortData(streamName, NULL);
		}
	}
}

bool LoopGroup::createInputDialog() 
{
	QWidget * w = new GUILoopGroup(this);
	w->show();
	return true;
}
