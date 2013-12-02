/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2012-2013  Christian Urich

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

#include "removecomponent.h"

DM_DECLARE_NODE_NAME(RemoveComponent,Modules)

RemoveComponent::RemoveComponent()
{
	this->remove_name = "";
    this->selectedattr = true;
	this->addParameter("View", DM::STRING, &this->remove_name);
    this->addParameter("Delete according to selected attribute", DM::BOOL, &this->selectedattr);


}

void RemoveComponent::init() {
	if (this->remove_name.empty())
		return;

	this->view_remove = DM::View(this->remove_name, DM::COMPONENT, DM::MODIFY);

	if (selectedattr)
		this->view_remove.addAttribute("selected", DM::Attribute::NOTYPE, DM::MODIFY);

	std::vector<DM::View> datastream;
	datastream.push_back(view_remove);
	this->addData("city", datastream);
}

void RemoveComponent::run() 
{
	DM::System * city = this->getData("city");

	std::vector<DM::Component*> comps = city->getAllComponentsInView(this->view_remove);
	DM::Logger(DM::Debug)  << "Elements in View before" << comps.size();
	foreach (DM::Component* cmp, comps)
	{
		if (this->selectedattr && cmp->getAttribute("selected")->getDouble() < 0.01)
			continue;
		city->removeComponentFromView(cmp, this->view_remove);

		foreach(DM::Attribute* attr, cmp->getAllAttributes())
		{
			if (attr->getType() == DM::Attribute::LINK) 
			{
				foreach(DM::Component* l_cmp, attr->getLinkedComponents())
					l_cmp->getAttribute(this->view_remove.getName())->removeLink(cmp);

				attr->clearLinks();
			}
		}
	}

	DM::Logger(DM::Debug)  << "Elements in View after" << city->getAllComponentsInView(this->view_remove).size();
}

string RemoveComponent::getHelpUrl()
{
	return "https://github.com/iut-ibk/DynaMind-BasicModules/blob/master/doc/RemoveComponent.md";
}


