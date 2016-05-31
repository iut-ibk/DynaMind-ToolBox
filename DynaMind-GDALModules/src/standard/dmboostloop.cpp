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

#include "dmboostloop.h"
#include "dmguiboostloop.h"
#include <dmlogger.h>
#include <dmcomponent.h>
#include <ogr_feature.h>
#include <iostream>
#include <string>

using namespace DM;

DM_DECLARE_CUSTOM_NODE_NAME(DMBoostLoop,Boost Loop, Test Modules)

DMBoostLoop::DMBoostLoop()
{
	this->GDALModule = true;
	runs = 1;
	currentRun = 0;

	this->addParameter("Runs", DM::INT, &runs);
    this->addParameter("Viewnames", DM::STRING, &viewnames);
    this->addParameter("Attributenames", DM::STRING, &attributenames);
    this->addParameter("Filternames", DM::STRING, &filternames);
	this->addParameter("writeStreams", DM::STRING_LIST, &writeStreams);
	this->addParameter("readStreams", DM::STRING_LIST, &readStreams);
}

void DMBoostLoop::run()
{
}

int DMBoostLoop::getGroupCounter()
{
	return currentRun;
}

void DMBoostLoop::init()
{
	currentRun = 0;

	foreach(std::string streamName, writeStreams)
		if(!initStream(streamName, true, true))
			setOutPortData(streamName, NULL);

	foreach(std::string streamName, readStreams)
		initStream(streamName, false, true);

}

bool DMBoostLoop::addStream(std::string name, bool write)
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

bool DMBoostLoop::initStream(std::string name, bool write, bool silent)
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

bool DMBoostLoop::removeStream(std::string name)
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

bool DMBoostLoop::condition()
{
    //check if view,attribute and statement are correctly set...
    if (viewnames.empty() or attributenames.empty() or filternames.empty()){
        DM::Logger(DM::Error) << "Missing variables";
        return false;
    }
    // why string in C++ is this complex?!
    // other operators not in use yet
    int equal = filternames.find("=");
    int unequal = filternames.find("<>");
    int zero = filternames.find("0");
    DM::Logger(DM::Standard) << "equal: " << equal;
    DM::Logger(DM::Standard) << "unequal: " << unequal;
    DM::Logger(DM::Standard) << "zero: " << zero;

    foreach(std::string streamName, writeStreams){
        GDALSystem * sys = (GDALSystem *) this->getGDALData(streamName);
        if (!sys){
            DM::Logger(DM::Error) << "system not set";
            return false;
        }
        DM::ViewContainer city(viewnames, DM::COMPONENT, DM::READ);
        city.setCurrentGDALSystem(sys);
        int value = 0;
        OGRFeature * f;
        const char * c = attributenames.c_str();
        while (f = city.getNextFeature()) {
            value = f->GetFieldAsInteger(c);}
            DM::Logger(DM::Standard) << "Indicator: " << value;
            DM::Logger(DM::Standard) << "Column: " << c;
        // we can't use a string as an if statement, so we have the following;
        if((unequal!=-1)and(zero!=-1))
        {if(value!=0){DM::Logger(DM::Error) << "Quit Loop"<<c<<"<>0";return false;}}
        if((equal!=-1)and(zero!=-1))
        {if(value==0){DM::Logger(DM::Error) << "Quit Loop"<<c<<"=0";return false;}}
      }

    if(currentRun++ < runs)
    {
        loopStreams();
        return true;
    }
    return false;
}

void DMBoostLoop::loopStreams()
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

bool DMBoostLoop::createInputDialog()
{
        QWidget * w = new DMGUIBoostLoop(this);
	w->show();
	return true;
}

string DMBoostLoop::getHelpUrl()
{
        return "/DynaMind-GDALModules/boostloop.html";
}
