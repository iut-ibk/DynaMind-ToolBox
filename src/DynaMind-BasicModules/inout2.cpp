#include "inout2.h"
#include <dmcomponent.h>
#include <dmsystem.h>
#include <dmnode.h>


DM_DECLARE_NODE_NAME( InOut2,Modules )
InOut2::InOut2() {


    Logger(Debug) << "Create InOut";
    std::vector<DM::View> views;
    inlets = DM::View("Inlets", DM::NODE, DM::READ);
    inlets.getAttribute("A");
    inlets.getAttribute("B");
    inlets.getAttribute("C");
    inlets.getAttribute("D");
    inlets.addAttribute("F");
    conduits = DM::View("Conduits", DM::EDGE,  DM::READ);
    conduits.addAttribute("F");
    //streets = DM::View("STREET", DM::EDGE,  DM::READ);

    views.push_back(inlets);
    views.push_back(conduits);
    //views.push_back(streets);

    this->addData("Inport", views);
    a = 0;
    this->addParameter("a",DM::DOUBLE, &a);
    a = 5;

}



void InOut2::run() {


    int test = a;
    Logger(Debug) << "Run InOut";
    Logger(Debug) << this->getParameterAsString("a");
    sys_in = this->getData("Inport");

    std::map<std::string, DM::Node*> all_nodes = sys_in->getAllNodes();

    for (std::map<std::string, DM::Node*>::const_iterator it = all_nodes.begin(); it != all_nodes.end(); ++it)
    {
        DM::Node * n = it->second;
        //Logger(Debug) << n->getName() << n->getX() << n->getY() << n->getZ();
    }

    DM::Node * n1 = sys_in->addNode(0,0,2, inlets);
    DM::Node * n2 = sys_in->addNode(0,0,3, inlets);

    double b = 0;

    std::map<std::string, DM::System*> subs = sys_in->getAllSubSystems();

    for (std::map<std::string, DM::System*>::const_iterator it = subs.begin(); it != subs.end(); ++it)
    {
        DM::System * s = it->second;
        Logger(Debug) << s->getUUID() << " " << s->getAllNodes().size() << " " << s->getAllEdges().size();
    }


    sys_in->addEdge(n1, n2, conduits);

    std::vector<std::string> nstreets = sys_in->getUUIDsOfComponentsInView(streets);
    foreach (std::string str, nstreets) {
        Edge * e = sys_in->getEdge(str);
        e->getAllAttributes();
    }


}

InOut2::~InOut2() {
    Logger(Debug) << "Destructor InOut";
}
