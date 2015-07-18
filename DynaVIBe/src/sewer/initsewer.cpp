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

#include <initsewer.h>

//DynaMind includes
#include <dmsystem.h>
#include <dmlogsink.h>
#include <sewerviewdef.h>

using namespace DM;

DM_DECLARE_NODE_NAME(InitSewerSystem,Sewersystems)

InitSewerSystem::InitSewerSystem()
{
	this->append=false;
	this->addParameter("AppendToExistingDataStream", DM::BOOL, &this->append);

}

void InitSewerSystem::init()
{
    S::ViewDefinitionHelper sd;
    this->data = sd.getAll(DM::WRITE);

	if (this->append)
		data.push_back( DM::View("dummy", SUBSYSTEM, READ));

    this->addData("Sewer", data);
}

void InitSewerSystem::run()
{
    this->getData("Sewer");
}
