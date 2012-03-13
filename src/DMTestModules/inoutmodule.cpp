#include "inoutmodule.h"
#include <dmcomponent.h>
#include <dmsystem.h>
#include <dmnode.h>


DM_DECLARE_NODE_NAME( InOut,Modules )
InOut::InOut() {

    Logger(Debug) << "Create InOut";
    std::vector<DM::View> views;
    inlets = DM::View("Inlets",DM::NODE, DM::MODIFY);
    inlets.getAttribute("A");
    inlets.getAttribute("B");
    inlets.addAttribute("C");
    conduits = DM::View("Conduits", DM::EDGE, DM::MODIFY);
    conduits.addAttribute("D");
    conduits.addAttribute("F");

    views.push_back(inlets);
    views.push_back(conduits);

    this->addData("Inport", views);
    a = 0;
    this->addParameter("a",DM::DOUBLE, &a);
    a = 5;




}



void InOut::run() {


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
        Logger(Debug) << s->getName() << " " << s->getAllNodes().size() << " " << s->getAllEdges().size();
    }

    /*for (int i = 0; i < test; i++) {
        for (int j = 0; j < test; j++) {
            for (int k = 0; k < test; k++)            {


                        b++;


            }
        }
    }
    Logger(Debug) << b;
    */

    sys_in->addEdge(n1, n2, conduits);


}

InOut::~InOut() {
    Logger(Debug) << "Destructor InOut";
}


