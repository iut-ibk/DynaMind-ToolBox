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

#ifndef CONTAINERGROUP_H
#define CONTAINERGROUP_H

#include "dmcompilersettings.h"
#include "dmgroup.h"
#include <set>

using namespace DM;

class GUILoopGroup;

class DM_HELPER_DLL_EXPORT ContainerGroup : public  Group 
{
	DM_DECLARE_NODE(ContainerGroup)

	friend class GUILoopGroup;

public:
	ContainerGroup();
	void run();
	virtual void init();
	//virtual bool createInputDialog();
	
	std::vector<std::string> inStreams;
	std::vector<std::string> outStreams;
	
	bool addStream(std::string name, bool out);
	bool removeStream(std::string name);
private:
	bool initStream(std::string name, bool out, bool silent = false);
};


#endif // CONTAINERGROUP_H
