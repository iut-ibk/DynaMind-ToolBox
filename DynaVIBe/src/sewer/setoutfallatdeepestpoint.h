/**
 * @file
 * @author  Michael Mair <michael.mair@uibk.ac.at>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2012  Michael Mair

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

#ifndef SetOutfallAtDeepestPoint_H
#define SetOutfallAtDeepestPoint_H

#include <dmmodule.h>
#include <dm.h>

class SetOutfallAtDeepestPoint : public DM::Module
{
	DM_DECLARE_NODE(SetOutfallAtDeepestPoint)

	vector<DM::View> views;
	DM::System * sys;

public:
	SetOutfallAtDeepestPoint();
	void init();
	void run();
};

#endif // SetOutfallAtDeepestPoint_H
