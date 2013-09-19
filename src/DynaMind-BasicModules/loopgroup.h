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

#ifndef LOOPGROUP_H
#define LOOPGROUP_H

#include "dmcompilersettings.h"
#include "dmgroup.h"
#include <set>

using namespace DM;

class GUILoopGroup;

class DM_HELPER_DLL_EXPORT LoopGroup : public  Group 
{
	DM_DECLARE_NODE(LoopGroup)

	friend class GUILoopGroup;

public:
	LoopGroup();
	virtual ~LoopGroup(){}
	void run();
	virtual void init();
	virtual bool createInputDialog();
	
	std::vector<std::string> readStreams;
	std::vector<std::string> writeStreams;
	
	bool initStream(std::string name, bool write, bool silent = false);
	bool addStream(std::string name, bool write);
	bool removeStream(std::string name);

	int runs;
	int currentRun;	// [0,runs-1]

	virtual bool condition();
	int getGroupCounter();
protected:
	void loopStreams();
};


#endif // LOOPGROUP_H
