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

#ifndef DMGROUP_H
#define DMGROUP_H

#include <dmmodule.h>

namespace DM {

class DM_HELPER_DLL_EXPORT Group: public Module
{
	friend Simulation;
public:
	virtual void init();
	bool isGroup(){return true;}
	/*backwards compatible returns counter of group until groups have more power. Returns -1 if not set*/
	virtual int getGroupCounter() {return -1;}
private:
	// as a standard we implement a single-run condition
	bool singleRunCondition;

	virtual bool condition();
	// a temporary storage for all streams and viewnames in the stream up to this module
	// it is updated by simulation::checkModuleStream
	std::map<std::string, std::map<std::string,View> > outStreamViews;
};

}


#endif // DMGROUP_H
