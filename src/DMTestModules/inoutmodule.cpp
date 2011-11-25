#include "inoutmodule.h"
#include <DMcomponent.h>
#include <DMsystem.h>
#include <DMnode.h>


VIBe_DECLARE_NODE_NAME( InOut,Modules )
InOut::InOut() {

    Logger(Debug) << "Create InOut";
    std::vector<DM::View> views;
    DM::View inlets = DM::View("Inlets");
    inlets.getComponent(DM::NODE);
    inlets.getAttributes("A");
    inlets.getAttributes("B");
    DM::View conduits = DM::View("Conduits");
    conduits.addComponent(DM::EDGE);

    views.push_back(inlets);
    views.push_back(conduits);

    this->addData("Inport", views);
    a = 0;
    this->addParameter("a",VIBe2::DOUBLE, &a);
    a = 5;
}



void InOut::run() {


    int test = 1500;
    Logger(Debug) << "Run InOut";
    Logger(Debug) << this->getParameterAsString("a");
    sys_in = this->getData("Inport");

    std::map<std::string, DM::Node*> all_nodes = sys_in->getAllNodes();

    for (std::map<std::string, DM::Node*>::const_iterator it = all_nodes.begin(); it != all_nodes.end(); ++it) {
        DM::Node * n = it->second;
        Logger(Debug) << n->getName() << n->getX() << n->getY() << n->getZ();
    }

    DM::Node * n1 = sys_in->addNode(0,0,2, "Inlets");
    DM::Node * n2 = sys_in->addNode(0,0,3, "Inlets");

    double b = 0;

    for (int i = 0; i < test; i++) {
        for (int j = 0; j < test; j++) {
            for (int k = 0; k < test; k++)            {


                        b++;


            }
        }
    }
    Logger(Debug) << b;

    sys_in->addEdge(n1, n2, "Conduits");


}

InOut::~InOut() {
    Logger(Debug) << "Destructor InOut";
}
