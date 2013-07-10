/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2012  Christian Urich

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

#ifndef GROUPTEST_H
#define GROUPTEST_H

#include "dmcompilersettings.h"
#include "dmgroup.h"
#include "dm.h"
#include "dmview.h"
#include <iostream>
#include <vector>
using namespace DM;


/** @ingroup TestModules
 * @brief Creates a group. To get data in the group add a dynamic inport, same for out
 *
 */
class DM_HELPER_DLL_EXPORT GroupTest : public  Group 
{
	DM_DECLARE_NODE(GroupTest)
private:
	/*
	int Runs;
	int i;
	std::vector<DM::View> InViews;
	std::vector<DM::View> OutViews;

	std::vector<std::string> nameOfInViews;
	std::vector<std::string> nameOfOutViews;
	*/
public:
	GroupTest();
	virtual ~GroupTest(){}
	void run(){}
	void addReadStream(const std::string& name);
	void addWriteStream(const std::string& name);
};


#endif // GROUPTEST_H
