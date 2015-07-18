/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2013  Christian Urich

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

#include "addsinglecomponent.h"
#include <dm.h>

DM_DECLARE_NODE_NAME(AddSingleComponent, Modules)
AddSingleComponent::AddSingleComponent()
{
	this->ViewName = "";
	this->Type = "COMPONENT";
	this->dummy = false;

	this->addParameter("ViewName", DM::STRING, &this->ViewName);
	this->addParameter("Type", DM::STRING, &this->Type);
	this->addParameter("dummy", DM::BOOL, &this->dummy);

}

void AddSingleComponent::init()
{
	if (this->ViewName.empty())
		return;
	if (this->Type == "COMPONENT")
		this->ComponentView = DM::View(ViewName, DM::COMPONENT, DM::WRITE);
	else if (this->Type == "NODE") {
		this->ComponentView = DM::View(ViewName, DM::NODE, DM::WRITE);
	}
	else if (this->Type == "EDGE") {
		this->ComponentView = DM::View(ViewName, DM::EDGE, DM::WRITE);
	}
	else if (this->Type == "FACE") {
		this->ComponentView = DM::View(ViewName, DM::FACE, DM::WRITE);
	} else {
		DM::Logger(DM::Standard) << "Wrong Type";
		return;
	}

	std::vector<DM::View> datastream;
	datastream.push_back(ComponentView);

	this->addData("sys", datastream);

}

void AddSingleComponent::run()
{
	DM::System * sys = this->getData("sys");
	if (this->dummy)
		sys->addComponent(new DM::Component(), this->ComponentView);
}

std::string AddSingleComponent::getHelpUrl()
{
		return "https://github.com/iut-ibk/DynaMind-BasicModules/blob/master/doc/AddSingleComponent.md";
}

std::string AddSingleComponent::getViewName() const
{
	return ViewName;
}

void AddSingleComponent::setViewName(const std::string &value)
{
	ViewName = value;
}
