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

#include "dmloopgroup.h"
#include "dmguiloopgroup.h"
#include <dmlogger.h>

using namespace DM;

DM_DECLARE_CUSTOM_NODE_NAME(DMLoopGroup,For Loop, Workflow Control)

DMLoopGroup::DMLoopGroup()
{
	runs = 1;
	currentRun = 0;

	this->addParameter("Runs", DM::INT, &runs);
	this->addParameter("writeStreams", DM::STRING_LIST, &writeStreams);
	this->addParameter("readStreams", DM::STRING_LIST, &readStreams);
}

void DMLoopGroup::run()
{
}

int DMLoopGroup::getGroupCounter()
{
	return currentRun;
}

void DMLoopGroup::init()
{
	currentRun = 0;

	foreach(std::string streamName, writeStreams)
		if(!initStream(streamName, true, true))
			setOutPortData(streamName, NULL);

	foreach(std::string streamName, readStreams)
		initStream(streamName, false, true);
}

bool DMLoopGroup::addStream(std::string name, bool write)
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

bool DMLoopGroup::initStream(std::string name, bool write, bool silent)
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

bool DMLoopGroup::removeStream(std::string name)
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

bool DMLoopGroup::condition()
{
	if(currentRun++ < runs)
	{
		loopStreams();
		return true;
	}
	return false;
}

void DMLoopGroup::loopStreams()
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

bool DMLoopGroup::createInputDialog()
{
	QWidget * w = new DMGUILoopGroup(this);
	w->show();
	return true;
}

string DMLoopGroup::getHelpUrl()
{
	return "/DynaMind-GDALModules/loopgroup.html";
}
