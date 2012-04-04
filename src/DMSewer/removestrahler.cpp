#include "removestrahler.h"

DM_DECLARE_NODE_NAME(RemoveStrahler, Sewer)
RemoveStrahler::RemoveStrahler()
{
    this->junctions = DM::View("JUNCTION", DM::NODE, DM::MODIFY);
    this->conduits = DM::View("CONDUIT", DM::EDGE, DM::MODIFY);
    this->conduits.getAttribute("Strahler");

    std::vector<DM::View> data;
    data.push_back(this->junctions);
    data.push_back(this->conduits);


    this->addData("SEWER", data);


}

void RemoveStrahler::run()
{
    DM::System * sys = this->getData("SEWER");

    std::vector<std::string> condis = sys->getNamesOfComponentsInView(conduits);

    foreach (std::string s, condis) {
        DM::Edge * c = sys->getEdge(s);
        if (c->getAttribute("Strahler")->getDouble() < 2) {
            sys->removeComponentFromView(c, conduits);
            DM::Node * nstart = sys->getNode(c->getStartpointName());
            sys->removeComponentFromView(nstart, junctions);

        }
    }

}
