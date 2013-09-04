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
	this->addParameter("View", DM::STRING, &this->remove_name);


}

void RemoveComponent::init() {
	if (this->remove_name.empty())
		return;

	this->view_remove = DM::View(this->remove_name, DM::COMPONENT, DM::MODIFY);
	this->view_remove.getAttribute("selected");
	std::vector<DM::View> datastream;
	datastream.push_back(view_remove);
	this->addData("city", datastream);
}

void RemoveComponent::run() {
	DM::System * city = this->getData("city");

	std::vector<std::string> uuids = city->getUUIDs(this->view_remove);
	DM::Logger(DM::Debug)  << "Elements in View before" << uuids.size();
	foreach (std::string uuid, uuids) {
		DM::Component * cmp = city->getComponent(uuid);
		if (cmp->getAttribute("selected")->getDouble() < 0.01)
			continue;
		city->removeComponentFromView(cmp, this->view_remove);

		//remove
		std::map<std::string, DM::Attribute*> attr_map = cmp->getAllAttributes();
		for (std::map<std::string, DM::Attribute*>::const_iterator it = attr_map.begin(); it != attr_map.end(); ++it) {
			DM::Attribute * attr = it->second;
			if (attr->getType() == DM::Attribute::LINK) {
				std::vector<DM::LinkAttribute> links = attr->getLinks();
				foreach (DM::LinkAttribute link, links) {
					DM::Component * l_cmp = city->getComponent(link.uuid);
					if (!l_cmp) continue;
					std::vector<DM::LinkAttribute> cmp_links = l_cmp->getAttribute(this->view_remove.getName())->getLinks();

					std::vector<DM::LinkAttribute> cmp_links_new;

					foreach (DM::LinkAttribute l, cmp_links) {
						if (l.uuid != cmp->getUUID() || l.viewname != this->view_remove.getName()) cmp_links_new.push_back(l);
					}
					DM::Logger(DM::Debug) << "Remove links " << attr->getName() << "\t" << cmp_links.size() << "/" << cmp_links_new.size();

					l_cmp->getAttribute(this->view_remove.getName())->setLinks(cmp_links_new);
				}

				attr->setLinks(std::vector<DM::LinkAttribute>());

			}
		}
	}

	DM::Logger(DM::Debug)  << "Elements in View after" << city->getUUIDs(this->view_remove).size();

}

string RemoveComponent::getHelpUrl()
{
	return "https://github.com/iut-ibk/DynaMind-BasicModules/blob/master/doc/RemoveComponent.md";
}


