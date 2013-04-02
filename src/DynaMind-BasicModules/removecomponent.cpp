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
                    attr->setLinks(cmp_links_new);

                }

                attr->setLinks(std::vector<DM::LinkAttribute>());
            }
        }
    }

}


